#!/usr/bin/env python3
"""
Generate JavaScript natives module from nativedb/natives.json

Transforms native names from UPPER_CASE to camelCase and generates
wrapper functions that call Native.invoke with the correct hash.
"""

import json
import os
import re
import sys
from pathlib import Path

# JavaScript reserved keywords that cannot be used as parameter names
JS_RESERVED_KEYWORDS = {
    # Current reserved words
    'break', 'case', 'catch', 'continue', 'debugger', 'default', 'delete', 'do',
    'else', 'finally', 'for', 'function', 'if', 'in', 'instanceof', 'new',
    'return', 'switch', 'this', 'throw', 'try', 'typeof', 'var', 'void',
    'while', 'with',
    # Future reserved words
    'class', 'const', 'enum', 'export', 'extends', 'import', 'super',
    # Strict mode reserved words
    'implements', 'interface', 'let', 'package', 'private', 'protected',
    'public', 'static', 'yield',
    # Also avoid these common identifiers
    'null', 'true', 'false', 'undefined', 'NaN', 'Infinity',
}

def sanitize_param_name(name: str) -> str:
    """Sanitize a parameter name, handling reserved keywords."""
    # First sanitize invalid characters
    name = re.sub(r'[^a-zA-Z0-9_]', '_', name)
    if name and name[0].isdigit():
        name = '_' + name

    # Check for reserved keywords and rename
    if name.lower() in JS_RESERVED_KEYWORDS or name in JS_RESERVED_KEYWORDS:
        name = name + 'Value'

    return name

def upper_to_camel_case(name: str) -> str:
    """Convert UPPER_CASE or UPPER_PASCAL_CASE to camelCase.

    Examples:
        GET_GAME_TIMER -> getGameTimer
        SET_PED_COORDS -> setPedCoords
        _GET_SOMETHING -> getSomething (leading underscore stripped)
        _BG_SET_TEXT_SCALE -> bgSetTextScale
    """
    # Strip leading underscores (they're internal naming convention, not needed in JS API)
    name = name.lstrip('_')

    # Split by underscore and convert
    parts = name.lower().split('_')
    if not parts:
        return ''

    # First part stays lowercase, rest get capitalized first letter
    result = parts[0] + ''.join(word.capitalize() for word in parts[1:])
    return result

def get_invoke_method(return_type: str) -> str:
    """Determine which Native.invoke method to use based on return type."""
    if return_type == 'float':
        return 'invokeFloat'
    elif return_type == 'Vector3':
        return 'invokeVector3'
    else:
        return 'invoke'

def is_output_param(param_type: str) -> bool:
    """Check if a parameter is an output parameter (pointer type)."""
    return param_type.endswith('*') and param_type not in ['const char*', 'char*']

def generate_jsdoc(native: dict, camel_name: str) -> str:
    """Generate JSDoc comment for a native function."""
    lines = ['/**']

    # Add description from comment
    if native.get('comment'):
        comment = native['comment']
        # Strip any block comment patterns (like /*256*/) to avoid nested comment issues
        comment = strip_block_comments(comment)
        comment = comment.replace('\n', '\n * ')
        lines.append(f' * {comment}')

    # Add parameter documentation
    for param in native.get('params', []):
        param_type = param.get('type', 'any')
        param_name = param.get('name', 'param')
        js_type = map_type_to_jsdoc(param_type)
        lines.append(f' * @param {{{js_type}}} {param_name}')

    # Add return type
    return_type = native.get('return_type', 'void')
    if return_type != 'void':
        js_return = map_type_to_jsdoc(return_type)
        lines.append(f' * @returns {{{js_return}}}')

    lines.append(' */')
    return '\n'.join(lines)

def map_type_to_jsdoc(native_type: str) -> str:
    """Map native types to JSDoc types."""
    type_map = {
        'int': 'number',
        'float': 'number',
        'BOOL': 'boolean',
        'Hash': 'number',
        'const char*': 'string',
        'char*': 'string',
        'void': 'void',
        'Vector3': 'Vector3',
        'Vector3*': 'Vector3',
        'Any': 'any',
        'Any*': 'any',
    }

    # Handle pointer types
    if native_type.endswith('*') and native_type not in type_map:
        base_type = native_type[:-1]
        return f'{type_map.get(base_type, "number")}' # Output params

    # Handle handle types (Ped, Vehicle, etc.) - they're all numbers
    if native_type in type_map:
        return type_map[native_type]

    return 'number'  # Default for handles

def generate_native_function(hash_str: str, native: dict) -> str:
    """Generate a JavaScript function for a native."""
    name = native.get('name', '')
    if not name:
        return ''

    camel_name = upper_to_camel_case(name)
    params = native.get('params', [])
    return_type = native.get('return_type', 'void')
    invoke_method = get_invoke_method(return_type)

    # Generate parameter names
    param_names = []
    for param in params:
        param_name = param.get('name', 'param')
        # Sanitize parameter name (replace invalid chars and reserved keywords)
        param_name = sanitize_param_name(param_name)
        param_names.append(param_name)

    # Handle duplicate parameter names
    seen = {}
    unique_names = []
    for name in param_names:
        if name in seen:
            seen[name] += 1
            unique_names.append(f'{name}{seen[name]}')
        else:
            seen[name] = 0
            unique_names.append(name)
    param_names = unique_names

    # Generate JSDoc
    # jsdoc = generate_jsdoc(native, camel_name)

    # Generate function
    params_str = ', '.join(param_names)

    # Build the invoke call
    invoke_args = [hash_str]  # Hash is first argument
    invoke_args.extend(param_names)
    invoke_args_str = ', '.join(invoke_args)

    # Generate the function body
    # Use BigInt literal (suffix 'n') for 64-bit hashes to avoid precision loss
    hash_bigint = f'{hash_str}n'
    invoke_args_bigint = [hash_bigint]
    invoke_args_bigint.extend(param_names)
    invoke_args_str_bigint = ', '.join(invoke_args_bigint)

    if return_type == 'void':
        body = f'Native.{invoke_method}({invoke_args_str_bigint});'
    elif return_type == 'BOOL':
        body = f'return !!Native.{invoke_method}({invoke_args_str_bigint});'
    else:
        body = f'return Native.{invoke_method}({invoke_args_str_bigint});'

    return f'''
export function {camel_name}({params_str}) {{
    {body}
}}
'''

def generate_natives_module(natives_data: dict) -> str:
    """Generate the complete natives module."""
    lines = [
        '// Auto-generated natives module',
        '// Do not edit manually - regenerate with tools/codegen/generate_natives.py',
        '',
        '// Native is provided by the runtime',
        '',
    ]

    # Track all generated function names to avoid duplicates
    generated_names = set()

    # Process each namespace
    for namespace, natives in sorted(natives_data.items()):
        lines.append(f'// ============================================================================')
        lines.append(f'// {namespace}')
        lines.append(f'// ============================================================================')
        lines.append('')

        for hash_str, native in sorted(natives.items(), key=lambda x: x[1].get('name', '')):
            name = native.get('name', '')
            if not name:
                continue

            camel_name = upper_to_camel_case(name)

            # Skip duplicates
            if camel_name in generated_names:
                continue
            generated_names.add(camel_name)

            func_code = generate_native_function(hash_str, native)
            if func_code:
                lines.append(func_code)

    return '\n'.join(lines)

def generate_natives_hash_map(natives_data: dict) -> str:
    """Generate a hash map for native lookups by name."""
    lines = [
        '// Auto-generated native hash map',
        '// Do not edit manually',
        '',
        'export const NATIVES = {',
    ]

    for namespace, natives in sorted(natives_data.items()):
        for hash_str, native in sorted(natives.items(), key=lambda x: x[1].get('name', '')):
            name = native.get('name', '')
            if not name:
                continue
            lines.append(f'    {name}: {hash_str},')

    lines.append('};')
    lines.append('')

    return '\n'.join(lines)


def get_cpp_return_type(return_type: str) -> str:
    """Map native return type to C++ NativeReturnType enum value."""
    type_map = {
        'void': 'Void',
        'BOOL': 'Bool',
        'float': 'Float',
        'Vector3': 'Vector3',
        'const char*': 'String',
        'char*': 'String',
    }
    return type_map.get(return_type, 'Int')


def generate_natives_cpp(natives_data: dict) -> str:
    """Generate C++ JSNativesGenerated.cpp with native registration."""
    lines = [
        '// Auto-generated natives registration',
        '// Do not edit manually - regenerate with tools/codegen/generate_natives.py',
        '',
        '#include "stdafx.h"',
        '#include "JSNativesGenerated.h"',
        '#include "JSNativeBindings.h"',
        '#include "JSRuntime.h"',
        '',
        '#include <JavaScriptCore/JavaScript.h>',
        '#include <vector>',
        '',
        'namespace rdr2js {',
        '',
        '// Forward declarations',
        'extern thread_local JSRuntime* g_currentRuntime;',
        'std::string GetStringFromJSValue(JSContextRef ctx, JSValueRef value);',
        'JSStringRef CreateJSString(const std::string& str);',
        'void SetNumberProperty(JSContextRef ctx, JSObjectRef obj, const char* name, double value);',
        '',
        '// Static class for native function objects',
        'static JSClassRef s_nativeFunctionClass = nullptr;',
        '',
        '// Prevent double-free by tracking allocated NativeInfo',
        'static std::vector<NativeInfo*> s_allocatedNativeInfos;',
        '',
        '// Native context for calling game functions',
        'struct NativeContext {',
        '    uint64_t* retVal = stack;',
        '    uint64_t argCount = 0;',
        '    uint64_t* stackPtr = stack;',
        '    uint64_t dataCount = 0;',
        '    uint64_t spaceForResults[24];',
        '    uint64_t stack[24]{ 0 };',
        '',
        '    void Reset() {',
        '        argCount = 0;',
        '        dataCount = 0;',
        '        memset(stack, 0, sizeof(stack));',
        '    }',
        '',
        '    void Push(uint64_t value) {',
        '        stack[argCount++] = value;',
        '    }',
        '',
        '    template<typename T>',
        '    T Result() {',
        '        return *reinterpret_cast<T*>(retVal);',
        '    }',
        '',
        '    void CopyResults() {',
        '        uint64_t a1 = (uint64_t)this;',
        '        uint64_t result;',
        '        for (; *(uint32_t*)(a1 + 24); *(uint32_t*)(*(uint64_t*)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) + 16i64) = result) {',
        '            --*(uint32_t*)(a1 + 24);',
        '            **(uint32_t**)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) = *(uint32_t*)(a1 + 16 * (*(signed int*)(a1 + 24) + 4i64));',
        '            *(uint32_t*)(*(uint64_t*)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) + 8i64) = *(uint32_t*)(a1 + 16i64 * *(signed int*)(a1 + 24) + 68);',
        '            result = *(unsigned int*)(a1 + 16i64 * *(signed int*)(a1 + 24) + 72);',
        '        }',
        '        --*(uint32_t*)(a1 + 24);',
        '    }',
        '};',
        '',
        'typedef void(__cdecl* NativeHandler)(NativeContext* context);',
        '',
        '// SEH-safe helper functions',
        'static bool SafeCallNativeHandler(NativeHandler handler, NativeContext* ctx, DWORD* exceptionCode) {',
        '    __try {',
        '        handler(ctx);',
        '        return true;',
        '    }',
        '    __except (EXCEPTION_EXECUTE_HANDLER) {',
        '        if (exceptionCode) *exceptionCode = GetExceptionCode();',
        '        return false;',
        '    }',
        '}',
        '',
        'static bool SafeCopyResults(NativeContext* ctx, DWORD* exceptionCode) {',
        '    __try {',
        '        ctx->CopyResults();',
        '        return true;',
        '    }',
        '    __except (EXCEPTION_EXECUTE_HANDLER) {',
        '        if (exceptionCode) *exceptionCode = GetExceptionCode();',
        '        return false;',
        '    }',
        '}',
        '',
        '// Single dispatcher callback for all native functions',
        'static JSValueRef NativeDispatcher(JSContextRef ctx, JSObjectRef function,',
        '    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {',
        '',
        '    // Get native info from function\'s private data',
        '    NativeInfo* info = static_cast<NativeInfo*>(JSObjectGetPrivate(function));',
        '    if (!info) {',
        '        *exception = JSValueMakeString(ctx, CreateJSString("Invalid native function"));',
        '        return JSValueMakeUndefined(ctx);',
        '    }',
        '',
        '    if (!g_currentRuntime) {',
        '        *exception = JSValueMakeString(ctx, CreateJSString("Runtime not available"));',
        '        return JSValueMakeUndefined(ctx);',
        '    }',
        '',
        '    // Get native function address',
        '    auto getNativeAddr = g_currentRuntime->GetNativeAddrFn();',
        '    if (!getNativeAddr) {',
        '        *exception = JSValueMakeString(ctx, CreateJSString("getNativeAddr function not set"));',
        '        return JSValueMakeUndefined(ctx);',
        '    }',
        '',
        '    uintptr_t nativeAddr = getNativeAddr(info->hash);',
        '    if (!nativeAddr) {',
        '        *exception = JSValueMakeString(ctx, CreateJSString("Native function not found"));',
        '        return JSValueMakeUndefined(ctx);',
        '    }',
        '',
        '    NativeHandler handler = reinterpret_cast<NativeHandler>(nativeAddr);',
        '',
        '    // Build native context with arguments',
        '    static thread_local NativeContext nativeCtx;',
        '    nativeCtx.Reset();',
        '',
        '    // Storage for string arguments - must live until after the native call completes',
        '    static thread_local std::vector<std::string> stringArgs;',
        '    stringArgs.clear();',
        '',
        '    // Push all arguments',
        '    for (size_t i = 0; i < argumentCount; i++) {',
        '        JSValueRef arg = arguments[i];',
        '',
        '        if (JSValueIsNumber(ctx, arg)) {',
        '            double num = JSValueToNumber(ctx, arg, nullptr);',
        '            if (num == static_cast<double>(static_cast<int64_t>(num))) {',
        '                nativeCtx.Push(static_cast<uint64_t>(static_cast<int64_t>(num)));',
        '            } else {',
        '                float f = static_cast<float>(num);',
        '                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&f));',
        '            }',
        '        } else if (JSValueIsBoolean(ctx, arg)) {',
        '            nativeCtx.Push(JSValueToBoolean(ctx, arg) ? 1ULL : 0ULL);',
        '        } else if (JSValueIsString(ctx, arg)) {',
        '            // Store string to keep it alive until native call completes',
        '            stringArgs.push_back(GetStringFromJSValue(ctx, arg));',
        '            nativeCtx.Push(reinterpret_cast<uint64_t>(stringArgs.back().c_str()));',
        '        } else if (JSValueIsNull(ctx, arg) || JSValueIsUndefined(ctx, arg)) {',
        '            nativeCtx.Push(0ULL);',
        '        } else if (JSValueIsObject(ctx, arg)) {',
        '            JSObjectRef obj = JSValueToObject(ctx, arg, nullptr);',
        '            ',
        '            // Check for native string handle (__nativePtr__ property)',
        '            JSStringRef ptrProp = JSStringCreateWithUTF8CString("__nativePtr__");',
        '            if (JSObjectHasProperty(ctx, obj, ptrProp)) {',
        '                // This is a native string handle - use the raw pointer',
        '                double ptr = JSValueToNumber(ctx, JSObjectGetProperty(ctx, obj, ptrProp, nullptr), nullptr);',
        '                nativeCtx.Push(static_cast<uint64_t>(ptr));',
        '                JSStringRelease(ptrProp);',
        '                continue;',
        '            }',
        '            JSStringRelease(ptrProp);',
        '            ',
        '            // Check for Vector3 (x, y, z properties)',
        '            JSStringRef xProp = JSStringCreateWithUTF8CString("x");',
        '            if (JSObjectHasProperty(ctx, obj, xProp)) {',
        '                JSStringRef yProp = JSStringCreateWithUTF8CString("y");',
        '                JSStringRef zProp = JSStringCreateWithUTF8CString("z");',
        '                float x = static_cast<float>(JSValueToNumber(ctx, JSObjectGetProperty(ctx, obj, xProp, nullptr), nullptr));',
        '                float y = static_cast<float>(JSValueToNumber(ctx, JSObjectGetProperty(ctx, obj, yProp, nullptr), nullptr));',
        '                float z = static_cast<float>(JSValueToNumber(ctx, JSObjectGetProperty(ctx, obj, zProp, nullptr), nullptr));',
        '                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&x));',
        '                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&y));',
        '                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&z));',
        '                JSStringRelease(yProp);',
        '                JSStringRelease(zProp);',
        '            } else {',
        '                nativeCtx.Push(0ULL);',
        '            }',
        '            JSStringRelease(xProp);',
        '        } else {',
        '            nativeCtx.Push(0ULL);',
        '        }',
        '    }',
        '',
        '    // Call native function',
        '    DWORD exceptionCode = 0;',
        '    if (!SafeCallNativeHandler(handler, &nativeCtx, &exceptionCode)) {',
        '        *exception = JSValueMakeString(ctx, CreateJSString("Native function crashed"));',
        '        return JSValueMakeUndefined(ctx);',
        '    }',
        '',
        '    SafeCopyResults(&nativeCtx, &exceptionCode);',
        '',
        '    // Return result based on type',
        '    switch (info->returnType) {',
        '        case NativeReturnType::Void:',
        '            return JSValueMakeUndefined(ctx);',
        '',
        '        case NativeReturnType::Bool:',
        '            return JSValueMakeBoolean(ctx, nativeCtx.Result<int>() != 0);',
        '',
        '        case NativeReturnType::Float: {',
        '            uint32_t bits = nativeCtx.Result<uint32_t>();',
        '            float f = *reinterpret_cast<float*>(&bits);',
        '            return JSValueMakeNumber(ctx, f);',
        '        }',
        '',
        '        case NativeReturnType::Vector3: {',
        '            float x = *reinterpret_cast<float*>(nativeCtx.retVal + 0);',
        '            float y = *reinterpret_cast<float*>((uintptr_t)nativeCtx.retVal + 8);',
        '            float z = *reinterpret_cast<float*>((uintptr_t)nativeCtx.retVal + 16);',
        '            JSObjectRef vec3 = JSObjectMake(ctx, nullptr, nullptr);',
        '            SetNumberProperty(ctx, vec3, "x", x);',
        '            SetNumberProperty(ctx, vec3, "y", y);',
        '            SetNumberProperty(ctx, vec3, "z", z);',
        '            return vec3;',
        '        }',
        '',
        '        case NativeReturnType::String: {',
        '            const char* str = nativeCtx.Result<const char*>();',
        '            if (str) {',
        '                // Create a native string handle object that preserves the raw pointer',
        '                // This allows the pointer to be passed back to other natives',
        '                JSObjectRef strHandle = JSObjectMake(ctx, nullptr, nullptr);',
        '                ',
        '                // Store raw pointer as __nativePtr__',
        '                JSStringRef ptrProp = JSStringCreateWithUTF8CString("__nativePtr__");',
        '                JSObjectSetProperty(ctx, strHandle, ptrProp,',
        '                    JSValueMakeNumber(ctx, static_cast<double>(reinterpret_cast<uint64_t>(str))),',
        '                    kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontEnum, nullptr);',
        '                JSStringRelease(ptrProp);',
        '                ',
        '                // Store string value for display/debugging',
        '                JSStringRef valueProp = JSStringCreateWithUTF8CString("value");',
        '                JSStringRef jsStr = JSStringCreateWithUTF8CString(str);',
        '                JSObjectSetProperty(ctx, strHandle, valueProp, JSValueMakeString(ctx, jsStr),',
        '                    kJSPropertyAttributeReadOnly, nullptr);',
        '                JSStringRelease(jsStr);',
        '                JSStringRelease(valueProp);',
        '                ',
        '                // Add toString method for console.log compatibility',
        '                JSStringRef toStringProp = JSStringCreateWithUTF8CString("toString");',
        '                JSStringRef toStringBody = JSStringCreateWithUTF8CString("return this.value;");',
        '                JSObjectRef toStringFn = JSObjectMakeFunction(ctx, toStringProp, 0, nullptr, toStringBody, nullptr, 1, nullptr);',
        '                JSObjectSetProperty(ctx, strHandle, toStringProp, toStringFn, kJSPropertyAttributeDontEnum, nullptr);',
        '                JSStringRelease(toStringBody);',
        '                JSStringRelease(toStringProp);',
        '                ',
        '                return strHandle;',
        '            }',
        '            return JSValueMakeNull(ctx);',
        '        }',
        '',
        '        case NativeReturnType::Int:',
        '        default:',
        '            return JSValueMakeNumber(ctx, static_cast<double>(nativeCtx.Result<int64_t>()));',
        '    }',
        '}',
        '',
        '// Ensure class is created',
        'static void EnsureNativeFunctionClass() {',
        '    if (!s_nativeFunctionClass) {',
        '        JSClassDefinition def = kJSClassDefinitionEmpty;',
        '        def.className = "NativeFunction";',
        '        def.callAsFunction = NativeDispatcher;',
        '        // Note: We don\'t set finalize - we manage NativeInfo lifetime ourselves',
        '        s_nativeFunctionClass = JSClassCreate(&def);',
        '    }',
        '}',
        '',
        '// Register a single native function',
        'static void RegisterNative(JSContextRef ctx, JSObjectRef nativesObj,',
        '    const char* name, uint64_t hash, NativeReturnType returnType, uint8_t paramCount) {',
        '    ',
        '    EnsureNativeFunctionClass();',
        '',
        '    NativeInfo* info = new NativeInfo{hash, returnType, paramCount};',
        '    s_allocatedNativeInfos.push_back(info);',
        '',
        '    JSObjectRef func = JSObjectMake(ctx, s_nativeFunctionClass, info);',
        '    JSStringRef propName = JSStringCreateWithUTF8CString(name);',
        '    JSObjectSetProperty(ctx, nativesObj, propName, func, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, nullptr);',
        '    JSStringRelease(propName);',
        '}',
        '',
        'JSClassRef GetNativeFunctionClass() {',
        '    return s_nativeFunctionClass;',
        '}',
        '',
        'static size_t s_registeredNativeCount = 0;',
        '',
        'size_t GetRegisteredNativeCount() {',
        '    return s_registeredNativeCount;',
        '}',
        '',
        '// ============================================================================',
        '// Generated native registrations',
        '// ============================================================================',
        '',
        'void RegisterGeneratedNatives(JSContextRef ctx, JSObjectRef nativesObj) {',
        '    using RT = NativeReturnType;',
        '',
    ]

    # Track generated names to avoid duplicates
    generated_names = set()
    count = 0

    for namespace, natives in sorted(natives_data.items()):
        lines.append(f'    // {namespace}')

        for hash_str, native in sorted(natives.items(), key=lambda x: x[1].get('name', '')):
            name = native.get('name', '')
            if not name:
                continue

            camel_name = upper_to_camel_case(name)

            # Skip duplicates
            if camel_name in generated_names:
                continue
            generated_names.add(camel_name)

            return_type = native.get('return_type', 'void')
            cpp_return_type = get_cpp_return_type(return_type)
            param_count = len(native.get('params', []))

            lines.append(f'    RegisterNative(ctx, nativesObj, "{camel_name}", {hash_str}ULL, RT::{cpp_return_type}, {param_count});')
            count += 1

        lines.append('')

    lines.append(f'    s_registeredNativeCount = {count};')
    lines.append('}')
    lines.append('')
    lines.append('} // namespace rdr2js')
    lines.append('')

    return '\n'.join(lines)


def generate_native_names_header(natives_data: dict) -> str:
    """Generate C++ NativeNames.h for the trainer."""
    lines = [
        '#pragma once',
        '// Auto-generated native names header',
        '// Do not edit manually - regenerate with tools/codegen/generate_natives.py',
        '',
        '#define REG(x,y) constexpr uint64_t x = y',
        '',
        'namespace N',
        '{',
        '',
    ]

    # Track generated names to avoid duplicates
    generated_names = set()

    for namespace, natives in sorted(natives_data.items()):
        lines.append(f'\t//Category: {namespace}')
        lines.append('')

        for hash_str, native in sorted(natives.items(), key=lambda x: x[1].get('name', '')):
            name = native.get('name', '')
            if not name:
                continue

            # Skip duplicates
            if name in generated_names:
                continue
            generated_names.add(name)

            lines.append(f'\tREG({name},{hash_str});')

    lines.append('}')
    lines.append('')

    return '\n'.join(lines)

def map_type_to_ts(native_type: str) -> str:
    """Map native types to TypeScript types."""
    type_map = {
        'int': 'number',
        'float': 'number',
        'BOOL': 'boolean',
        'Hash': 'number',
        'const char*': 'string',
        'char*': 'string',
        'void': 'void',
        'Vector3': 'Vector3',
        'Vector3*': 'Vector3',
        'Any': 'any',
        'Any*': 'any',
        'Entity': 'number',
        'Ped': 'number',
        'Vehicle': 'number',
        'Object': 'number',
        'Cam': 'number',
        'Player': 'number',
        'Blip': 'number',
        'Pickup': 'number',
        'Interior': 'number',
        'FireId': 'number',
        'ScrHandle': 'number',
        'ItemSet': 'number',
        'Volume': 'number',
        'AnimScene': 'number',
        'PersChar': 'number',
        'PopZone': 'number',
        'Prompt': 'number',
        'PropSet': 'number',
    }

    # Handle pointer types
    if native_type.endswith('*') and native_type not in type_map:
        base_type = native_type[:-1].strip()
        return type_map.get(base_type, 'number')

    if native_type in type_map:
        return type_map[native_type]

    return 'number'  # Default for handles


def generate_natives_dts(natives_data: dict) -> str:
    """Generate TypeScript declaration file for natives module."""
    lines = [
        '// Auto-generated TypeScript declarations for natives module',
        '// Do not edit manually - regenerate with tools/codegen/generate_natives.py',
        '',
        'declare module "natives" {',
        '    /** Vector3 type returned by some natives */',
        '    interface Vector3 {',
        '        x: number;',
        '        y: number;',
        '        z: number;',
        '    }',
        '',
        '    /** Native string handle returned by varString and similar natives */',
        '    interface NativeString {',
        '        /** Raw pointer to game memory - use when passing to other natives */',
        '        readonly __nativePtr__: number;',
        '        /** String value for display */',
        '        readonly value: string;',
        '        toString(): string;',
        '    }',
        '',
    ]

    # Track generated names to avoid duplicates
    generated_names = set()

    for namespace, natives in sorted(natives_data.items()):
        lines.append(f'    // {namespace}')

        for hash_str, native in sorted(natives.items(), key=lambda x: x[1].get('name', '')):
            name = native.get('name', '')
            if not name:
                continue

            camel_name = upper_to_camel_case(name)

            # Skip invalid identifiers (names starting with numbers/hash values)
            if not camel_name or camel_name[0].isdigit():
                continue

            # Skip duplicates
            if camel_name in generated_names:
                continue
            generated_names.add(camel_name)

            # Build parameter list
            params = native.get('params', [])
            param_strs = []
            is_variadic = native.get('variadic', False)
            for idx, param in enumerate(params):
                param_name = param.get('name', 'p')
                param_type = param.get('type', 'any')

                # Handle variadic parameters
                if param_name == '...' or (not param_type and is_variadic):
                    param_strs.append('...args: any[]')
                    continue

                ts_type = map_type_to_ts(param_type) if param_type else 'any'
                # Sanitize param name (some have reserved words or invalid names)
                if param_name in ['in', 'out', 'function', 'class', 'default', 'switch', 'case', 'return', 'new', 'this']:
                    param_name = param_name + '_'
                if not param_name or param_name.startswith('.'):
                    param_name = f'p{idx}'
                param_strs.append(f'{param_name}: {ts_type}')

            # Get return type
            return_type = native.get('return_type', 'void')
            ts_return = map_type_to_ts(return_type)
            # String returns are now NativeString objects
            if return_type in ['const char*', 'char*']:
                ts_return = 'NativeString'

            # Add JSDoc comment if available
            comment = native.get('comment', '')
            if comment:
                comment = comment.replace('*/', '').replace('/*', '').strip()
                if comment:
                    # Escape any problematic characters
                    comment = comment.replace('\n', ' ')
                    if len(comment) > 200:
                        comment = comment[:197] + '...'
                    lines.append(f'    /** {comment} */')

            lines.append(f'    export function {camel_name}({", ".join(param_strs)}): {ts_return};')

        lines.append('')

    lines.append('    const _default: {')
    lines.append('        [key: string]: (...args: any[]) => any;')
    lines.append('    };')
    lines.append('    export default _default;')
    lines.append('}')
    lines.append('')

    return '\n'.join(lines)


def generate_core_dts() -> str:
    """Generate TypeScript declaration file for core module."""
    return '''// Auto-generated TypeScript declarations for core module
// Do not edit manually - regenerate with tools/codegen/generate_natives.py

declare module "core" {
    /** Callback function type for tick callbacks */
    type TickCallback = () => void;

    /** Callback function type for key callbacks */
    type KeyCallback = (key: number) => void;

    /**
     * Register a callback to be called every game tick
     * @param callback Function to call every tick
     */
    export function addTickCallback(callback: TickCallback): void;

    /**
     * Register a callback to be called when a key is pressed
     * @param callback Function to call with the virtual key code
     */
    export function addKeyDownCallback(callback: KeyCallback): void;

    /**
     * Register a callback to be called when a key is released
     * @param callback Function to call with the virtual key code
     */
    export function addKeyUpCallback(callback: KeyCallback): void;
}
'''


def generate_globals_dts() -> str:
    """Generate TypeScript declaration file for global types and constants."""
    # Virtual key codes
    vk_codes = [
        ('VK_BACK', 0x08, 'Backspace'),
        ('VK_TAB', 0x09, 'Tab'),
        ('VK_RETURN', 0x0D, 'Enter'),
        ('VK_SHIFT', 0x10, 'Shift'),
        ('VK_CONTROL', 0x11, 'Ctrl'),
        ('VK_MENU', 0x12, 'Alt'),
        ('VK_PAUSE', 0x13, 'Pause'),
        ('VK_CAPITAL', 0x14, 'Caps Lock'),
        ('VK_ESCAPE', 0x1B, 'Escape'),
        ('VK_SPACE', 0x20, 'Space'),
        ('VK_PRIOR', 0x21, 'Page Up'),
        ('VK_NEXT', 0x22, 'Page Down'),
        ('VK_END', 0x23, 'End'),
        ('VK_HOME', 0x24, 'Home'),
        ('VK_LEFT', 0x25, 'Left Arrow'),
        ('VK_UP', 0x26, 'Up Arrow'),
        ('VK_RIGHT', 0x27, 'Right Arrow'),
        ('VK_DOWN', 0x28, 'Down Arrow'),
        ('VK_INSERT', 0x2D, 'Insert'),
        ('VK_DELETE', 0x2E, 'Delete'),
        ('VK_F1', 0x70, 'F1'),
        ('VK_F2', 0x71, 'F2'),
        ('VK_F3', 0x72, 'F3'),
        ('VK_F4', 0x73, 'F4'),
        ('VK_F5', 0x74, 'F5'),
        ('VK_F6', 0x75, 'F6'),
        ('VK_F7', 0x76, 'F7'),
        ('VK_F8', 0x77, 'F8'),
        ('VK_F9', 0x78, 'F9'),
        ('VK_F10', 0x79, 'F10'),
        ('VK_F11', 0x7A, 'F11'),
        ('VK_F12', 0x7B, 'F12'),
        ('VK_NUMPAD0', 0x60, 'Numpad 0'),
        ('VK_NUMPAD1', 0x61, 'Numpad 1'),
        ('VK_NUMPAD2', 0x62, 'Numpad 2'),
        ('VK_NUMPAD3', 0x63, 'Numpad 3'),
        ('VK_NUMPAD4', 0x64, 'Numpad 4'),
        ('VK_NUMPAD5', 0x65, 'Numpad 5'),
        ('VK_NUMPAD6', 0x66, 'Numpad 6'),
        ('VK_NUMPAD7', 0x67, 'Numpad 7'),
        ('VK_NUMPAD8', 0x68, 'Numpad 8'),
        ('VK_NUMPAD9', 0x69, 'Numpad 9'),
        ('VK_MULTIPLY', 0x6A, 'Numpad *'),
        ('VK_ADD', 0x6B, 'Numpad +'),
        ('VK_SUBTRACT', 0x6D, 'Numpad -'),
        ('VK_DECIMAL', 0x6E, 'Numpad .'),
        ('VK_DIVIDE', 0x6F, 'Numpad /'),
    ]

    lines = [
        '// Auto-generated TypeScript declarations for global types',
        '// Do not edit manually - regenerate with tools/codegen/generate_natives.py',
        '',
        '/** Hash utility for converting strings to game hashes */',
        'declare const Hash: {',
        '    /**',
        '     * Convert a string to a joaat hash (Jenkins one-at-a-time)',
        '     * @param str String to hash',
        '     * @returns Hash value as number',
        '     */',
        '    joaat(str: string): number;',
        '};',
        '',
        '/** Global script variables access */',
        'declare const Global: {',
        '    /**',
        '     * Get an integer from a global variable',
        '     * @param index Global variable index',
        '     * @returns The integer value',
        '     */',
        '    getInt(index: number): number;',
        '',
        '    /**',
        '     * Set an integer in a global variable',
        '     * @param index Global variable index',
        '     * @param value Value to set',
        '     */',
        '    setInt(index: number, value: number): void;',
        '',
        '    /**',
        '     * Get a float from a global variable',
        '     * @param index Global variable index',
        '     * @returns The float value',
        '     */',
        '    getFloat(index: number): number;',
        '',
        '    /**',
        '     * Set a float in a global variable',
        '     * @param index Global variable index',
        '     * @param value Value to set',
        '     */',
        '    setFloat(index: number, value: number): void;',
        '};',
        '',
        '// Virtual Key Codes',
    ]

    for name, value, comment in vk_codes:
        lines.append(f'/** {comment} key */')
        lines.append(f'declare const {name}: {value};')

    lines.append('')
    lines.append('// Letter keys (A-Z are 0x41-0x5A)')
    for i, letter in enumerate('ABCDEFGHIJKLMNOPQRSTUVWXYZ'):
        lines.append(f'declare const VK_{letter}: {0x41 + i};')

    lines.append('')
    lines.append('// Number keys (0-9 are 0x30-0x39)')
    for i in range(10):
        lines.append(f'declare const VK_{i}: {0x30 + i};')

    lines.append('')

    return '\n'.join(lines)


def main():
    # Find the project root
    script_dir = Path(__file__).parent
    project_root = script_dir.parent.parent

    natives_json_path = project_root / 'nativedb' / 'natives.json'
    js_output_dir = project_root / 'rdrhook' / 'src' / 'js' / 'modules'
    js_src_dir = project_root / 'rdrhook' / 'src' / 'js'
    trainer_output_dir = project_root / 'scripts' / 'trainer' / 'src' / 'core'

    if not natives_json_path.exists():
        print(f'Error: natives.json not found at {natives_json_path}')
        sys.exit(1)

    # Create output directories if needed
    js_output_dir.mkdir(parents=True, exist_ok=True)
    trainer_output_dir.mkdir(parents=True, exist_ok=True)

    # Load natives.json
    print(f'Loading {natives_json_path}...')
    with open(natives_json_path, 'r', encoding='utf-8') as f:
        natives_data = json.load(f)

    # Count natives
    total_natives = sum(len(natives) for natives in natives_data.values())
    print(f'Found {total_natives} natives in {len(natives_data)} namespaces')

    # Note: natives.js and natives_hashes.js are no longer generated
    # Natives are now registered directly in C++ via JSNativesGenerated.cpp

    # Generate C++ NativeNames.h for trainer
    print('Generating NativeNames.h...')
    native_names_header = generate_native_names_header(natives_data)

    native_names_path = trainer_output_dir / 'NativeNames.h'
    with open(native_names_path, 'w', encoding='utf-8') as f:
        f.write(native_names_header)
    print(f'Written to {native_names_path}')

    # Generate C++ JSNativesGenerated.cpp for rdrhook
    print('Generating JSNativesGenerated.cpp...')
    natives_cpp = generate_natives_cpp(natives_data)

    natives_cpp_path = js_src_dir / 'JSNativesGenerated.cpp'
    with open(natives_cpp_path, 'w', encoding='utf-8') as f:
        f.write(natives_cpp)
    print(f'Written to {natives_cpp_path}')

    # Create types output directory
    types_output_dir = project_root / 'shared' / 'types'
    types_output_dir.mkdir(parents=True, exist_ok=True)

    # Generate TypeScript declarations for natives module
    print('Generating natives.d.ts...')
    natives_dts = generate_natives_dts(natives_data)

    natives_dts_path = types_output_dir / 'natives.d.ts'
    with open(natives_dts_path, 'w', encoding='utf-8') as f:
        f.write(natives_dts)
    print(f'Written to {natives_dts_path}')

    # Generate TypeScript declarations for core module
    print('Generating core.d.ts...')
    core_dts = generate_core_dts()

    core_dts_path = types_output_dir / 'core.d.ts'
    with open(core_dts_path, 'w', encoding='utf-8') as f:
        f.write(core_dts)
    print(f'Written to {core_dts_path}')

    # Generate TypeScript declarations for globals
    print('Generating globals.d.ts...')
    globals_dts = generate_globals_dts()

    globals_dts_path = types_output_dir / 'globals.d.ts'
    with open(globals_dts_path, 'w', encoding='utf-8') as f:
        f.write(globals_dts)
    print(f'Written to {globals_dts_path}')

    print('Done!')

if __name__ == '__main__':
    main()
