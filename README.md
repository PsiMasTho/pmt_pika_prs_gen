Pika Parser Generator

Skeletons
- `make install-skel` installs only the `skel/` files.
- By default, the CLI uses the installed skeleton root (`${CMAKE_INSTALL_PREFIX}/share/pika_parser_generator/skel`).
- Override skeleton location at runtime with `--skel-dir /path/to/skel`.

VS Code syntax highlighting for .pika
- A minimal local extension lives in `pika-vscode/`.
- Manual install (no packaging):
  - `mkdir -p ~/.vscode/extensions/pika-lang-0.0.1`
  - `cp -R pika-vscode/* ~/.vscode/extensions/pika-lang-0.0.1/`
  - Reload VS Code (`Developer: Reload Window`).
