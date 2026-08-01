#!/usr/bin/env python3
"""Generate native bindings from nativedb/natives.json.

WebAssembly mods consume typed Rust wrappers that all route through the
runtime's single generic native ABI.
"""

import json
import re
import sys
from pathlib import Path


RUST_KEYWORDS = {
    "as", "break", "const", "continue", "crate", "else", "enum", "extern",
    "false", "fn", "for", "if", "impl", "in", "let", "loop", "match",
    "mod", "move", "mut", "pub", "ref", "return", "self", "Self", "static",
    "struct", "super", "trait", "true", "type", "unsafe", "use", "where",
    "while", "async", "await", "dyn", "abstract", "become", "box", "do",
    "final", "macro", "override", "priv", "typeof", "unsized", "virtual",
    "yield", "try",
}

HANDLE_TYPES = {
    "Entity", "Ped", "Vehicle", "Object", "Cam", "Player", "Blip",
    "Pickup", "Interior", "FireId", "ScrHandle", "ItemSet", "Volume",
    "AnimScene", "PersChar", "PopZone", "Prompt", "PropSet",
}


def to_snake_case(name: str) -> str:
    name = name.lstrip("_")
    name = re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", name)
    name = re.sub(r"[^A-Za-z0-9_]+", "_", name).strip("_").lower()
    name = re.sub(r"_+", "_", name)
    if not name:
        return "unnamed"
    if name[0].isdigit():
        name = "native_" + name
    if name in RUST_KEYWORDS:
        name += "_"
    return name


def sanitize_parameter(name: str, index: int) -> str:
    if not name or name == "...":
        return f"p{index}"
    return to_snake_case(name)


def rust_base_type(native_type: str) -> str:
    if native_type == "int":
        return "i32"
    if native_type == "float":
        return "f32"
    if native_type == "BOOL":
        return "bool"
    if native_type == "Hash":
        return "Hash"
    if native_type == "Any":
        return "Any"
    if native_type == "Vector3":
        return "Vector3"
    if native_type in HANDLE_TYPES:
        return native_type
    if native_type in {"const char*", "char*"}:
        return "&str"
    return "Any"


def rust_parameter_type(native_type: str) -> str:
    if native_type.endswith("*") and native_type not in {"const char*", "char*"}:
        base = native_type[:-1]
        if base == "BOOL":
            return "&mut i32"
        return f"&mut {rust_base_type(base)}"
    if native_type == "Any":
        return "NativeArg"
    return rust_base_type(native_type)


def native_argument_expression(native_type: str, name: str) -> str:
    if native_type.endswith("*") and native_type not in {"const char*", "char*"}:
        return f"NativeArg::in_out({name})"
    if native_type in {"const char*", "char*"}:
        return f"NativeArg::string({name})"
    if native_type == "float":
        return f"NativeArg::float({name})"
    if native_type == "BOOL":
        return f"NativeArg::boolean({name})"
    if native_type == "Hash":
        return f"NativeArg::hash({name})"
    if native_type == "Any":
        return name
    if native_type == "Vector3":
        return f"NativeArg::vector({name})"
    return f"NativeArg::int({name})"


def rust_return(native_type: str) -> tuple[str, str]:
    if native_type == "void":
        return "()", "map(|_| ())"
    if native_type == "BOOL":
        return "bool", "map(|result| result.as_bool())"
    if native_type == "float":
        return "f32", "map(|result| result.as_f32())"
    if native_type == "Vector3":
        return "Vector3", "map(|result| result.as_vector3())"
    if native_type in {"const char*", "char*"}:
        return "NativeString", "map(|result| result.as_string())"
    if native_type == "Hash":
        return "Hash", "map(|result| result.as_u32())"
    if native_type == "Any":
        return "Any", "map(|result| result.as_i64())"
    if native_type == "Any*":
        return "u64", "map(|result| result.as_u64())"
    if native_type == "int" or native_type in HANDLE_TYPES:
        return rust_base_type(native_type), "map(|result| result.as_i32())"
    return "Any", "map(|result| result.as_i64())"


def unique_parameter_names(params: list[dict]) -> list[str]:
    used: dict[str, int] = {}
    result = []
    for index, param in enumerate(params):
        base = sanitize_parameter(param.get("name", ""), index)
        count = used.get(base, 0)
        used[base] = count + 1
        result.append(base if count == 0 else f"{base}_{count + 1}")
    return result


def generate_rust_wrapper(hash_value: str, native: dict, function_name: str) -> str:
    params = native.get("params", [])
    variadic = native.get("variadic", False) or any(
        p.get("name") == "..." or not p.get("type") for p in params
    )
    fixed_params = [p for p in params if p.get("type")]
    parameter_names = unique_parameter_names(fixed_params)

    signature = [
        f"{name}: {rust_parameter_type(param.get('type', 'Any'))}"
        for name, param in zip(parameter_names, fixed_params)
    ]
    if variadic:
        signature.append("extra: &[NativeArg]")

    arguments = [
        native_argument_expression(param.get("type", "Any"), name)
        for name, param in zip(parameter_names, fixed_params)
    ]
    return_type, conversion = rust_return(native.get("return_type", "void"))

    lines = [
        "/// Invoke the corresponding RDR2 game native.",
        "///",
        "/// # Safety",
        "/// The caller must satisfy the game native's handle and pointer contract.",
        f"pub unsafe fn {function_name}({', '.join(signature)}) -> Result<{return_type}, NativeError> {{",
    ]
    if variadic:
        lines.append(f"    let mut arguments = Vec::with_capacity({len(arguments)} + extra.len());")
        for argument in arguments:
            lines.append(f"    arguments.push({argument});")
        lines.append("    arguments.extend_from_slice(extra);")
    else:
        lines.append(f"    let arguments = [{', '.join(arguments)}];")
    lines.append(
        f"    unsafe {{ native::invoke({hash_value}, &arguments) }}.{conversion}"
    )
    lines.append("}")
    return "\n".join(lines)


def generate_rust_natives(natives_data: dict) -> str:
    lines = [
        "// Auto-generated by tools/codegen/generate_natives.py.",
        "// Do not edit manually.",
        "#![allow(clippy::too_many_arguments)]",
        "#![allow(clippy::missing_safety_doc)]",
        "",
        "use crate::native::{self, NativeArg, NativeError, NativeString};",
        "use crate::types::*;",
        "",
    ]
    generated = set()
    count = 0
    for namespace, natives in sorted(natives_data.items()):
        lines.extend([
            "// ============================================================================",
            f"// {namespace}",
            "// ============================================================================",
            "",
        ])
        for hash_value, native in sorted(
            natives.items(), key=lambda item: item[1].get("name", "")
        ):
            name = native.get("name", "")
            if not name:
                continue
            function_name = to_snake_case(name)
            if function_name in generated:
                function_name = f"{to_snake_case(namespace)}_{function_name}"
            if function_name in generated:
                function_name = f"{function_name}_{hash_value[2:].lower()}"
            generated.add(function_name)
            lines.append(generate_rust_wrapper(hash_value, native, function_name))
            lines.append("")
            count += 1
    lines.append(f"// {count} named natives generated.")
    return "\n".join(lines)


def main() -> None:
    project_root = Path(__file__).resolve().parents[2]
    database_path = project_root / "nativedb" / "natives.json"
    if not database_path.exists():
        print(f"Error: {database_path} does not exist", file=sys.stderr)
        raise SystemExit(1)

    natives_data = json.loads(database_path.read_text(encoding="utf-8"))
    total = sum(len(namespace) for namespace in natives_data.values())
    print(f"Loaded {total} natives from {database_path}")

    rust_output = project_root / "scripts" / "rdr2-wasm" / "src" / "natives.rs"
    rust_output.parent.mkdir(parents=True, exist_ok=True)
    rust_output.write_text(
        generate_rust_natives(natives_data), encoding="utf-8", newline="\n"
    )
    print(f"Wrote {rust_output}")


if __name__ == "__main__":
    main()
