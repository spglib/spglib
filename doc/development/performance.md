# Performance

Benchmark performance of {func}`get_symmetry_dataset` for all cells in `test/data`:

```shell
pytest --benchmark-only --benchmark-columns=mean,stddev -s -v test/test_benchmark.py
```
