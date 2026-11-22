# Working on this plugin

This repository contains only the `skyrc_charger` Tessera plugin. Read the upstream repository's `AGENTS.md` and docs before firmware changes: https://github.com/MaxGramser/tessera-plugins

Validate from a clone of tessera-plugins with `python3 tools/check.py /path/to/skyrc_charger`. For a drawing/build preview use `python3 tools/preview.py /path/to/skyrc_charger --strict`; firmware builds and hardware testing are not replaced by the manifest checker.

Keep Home Assistant action permissions limited to the actions actually used. Never send charger-control actions while performing read-only tests. Keep IDs in example_layout.yaml tied to Home Assistant observations; replace them only after verifying the device registry.
