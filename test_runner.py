import onnxruntime as ort
import numpy as np
import sys
import os

def run_model(model_path):
    print(f"\n==> Running model: {model_path}")

    # Load session
    providers = ort.get_available_providers()
    if 'CUDAExecutionProvider' in providers:
        exec_providers = ['CUDAExecutionProvider']
    else:
        exec_providers = ['CPUExecutionProvider']
    print(f"Using providers: {exec_providers}")

    session = ort.InferenceSession(model_path, providers=exec_providers)

    # Get input info
    inputs_meta = session.get_inputs()
    inputs = {}
    for input_meta in inputs_meta:
        name = input_meta.name
        shape = [dim if isinstance(dim, int) else 1 for dim in input_meta.shape]
        dtype_str = input_meta.type
        if "float" in dtype_str:
            dtype = np.float32
        elif "uint8" in dtype_str:
            dtype = np.uint8
        else:
            raise RuntimeError(f"Unsupported dtype: {dtype_str}")
        dummy_input = np.zeros(shape, dtype=dtype)
        inputs[name] = dummy_input
        print(f"  Input '{name}': shape={shape}, dtype={dtype.__name__}")

    # Run inference
    outputs = session.run(None, inputs)

    # Show outputs
    for i, out in enumerate(outputs):
        print(f"  Output {i}: shape={out.shape}, dtype={out.dtype}")
        print(f"    preview: {out.flatten()[:8]}")

if __name__ == "__main__":
    if len(sys.argv) != 2 or not os.path.exists(sys.argv[1]):
        print("Usage: python test_runner.py /path/to/model.onnx")
        sys.exit(1)

    run_model(sys.argv[1])

