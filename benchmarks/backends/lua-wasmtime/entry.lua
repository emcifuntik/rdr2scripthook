local kernels = require("benchmark_kernels")
register_event("benchmark", kernels.run)
