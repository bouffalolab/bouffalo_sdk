# SDK documentation

Initialize the SDK submodules at the revision recorded by the SDK checkout
before building documentation:

```bash
git submodule update --init --recursive
make -C docs html SOURCEDIR=en
make -C docs html SOURCEDIR=zh
```

The AT command reference is included from the `atmodule` submodule. For an SDK
release, use its recorded submodule revision rather than updating AT to the
latest branch. This keeps the documentation aligned with the SDK release.
