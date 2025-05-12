import onnxruntime as ort
import numpy as np
import sys
import os

def run_with_trt(model_path):
    print(f"\n==> Running model with TensorRT: {model_path}")

    # 检查是否支持 TensorRT
    providers = ort.get_available_providers()
    if 'TensorrtExecutionProvider' not in providers:
        print("❌ TensorRT Execution Provider is NOT available.")
        print("Available providers:", providers)
        return

    # 启用 TensorRT 引擎缓存
    sess_opts = ort.SessionOptions()
    # sess_opts.enable_trt_engine_cache = True
    # sess_opts.trt_engine_cache_path = "./trt_cache"

    print("Creating session... (this may take a while the first time)")
    sess = ort.InferenceSession(model_path, sess_options=sess_opts, providers=['TensorrtExecutionProvider'])
    print("Session created.")

    # 获取输入信息并准备 dummy 输入
    inputs_meta = sess.get_inputs()
    inputs = {}
    for meta in inputs_meta:
        name = meta.name
        shape = [dim if isinstance(dim, int) else 1 for dim in meta.shape]
        dtype_str = meta.type
        dtype = np.float32 if "float" in dtype_str else np.uint8
        arr = np.zeros(shape, dtype=dtype)
        inputs[name] = arr
        print(f"  Input: {name}, shape: {shape}, dtype: {dtype.__name__}")

    print("Running inference...")
    outputs = sess.run(None, inputs)
    print("Inference done.\n")

    # 输出结果信息
    for i, out in enumerate(outputs):
        print(f"  Output {i}: shape={out.shape}, dtype={out.dtype}")
        print(f"    preview: {out.flatten()[:8]}")

if __name__ == "__main__":
    if len(sys.argv) != 2 or not os.path.exists(sys.argv[1]):
        print("Usage: python test_trt_runner.py /path/to/model.onnx")
        sys.exit(1)

    run_with_trt(sys.argv[1])

