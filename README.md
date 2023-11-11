# Tiger
Tiger is a really fast and powerful web server written purely in C.

## Getting Started
First download the source code and compile it.

```bash
git clone https://codeberg.com/kevidryon2/Tiger.git
cd Tiger
mkdir build
make
```

Since Tiger only consists of less than 1K lines of code, it is really fast to compile; by default, it will compile for the x86 architecture, but if you want to compile for another architecture (ARM32, ARM64, or Risc-V) just specify one of the following arguments to make: `arm`, `arm64`, `riscv`.

If you want to compile multiple architectures at once, choose between the following arguments:
`x86-arch`, `arm-arch`, `aarch64`, `riscv-arch`.

Then, create Tiger's directory structure and copy Tiger to it; in this example, we'll be using the `/srv` directory, but you can use any directory you want.

```bash
mkdir -p /srv/{public,cache,scripts,bin}
cp build/tiger-*_dynamic /src/bin
```

Lastly, start Tiger.

```bash
cd /srv/bin
export TIGER_PATH=..
./tiger
```

### Command-line arguments

You can specify a number of command-line arguments to disable certain features; like `tar`, command-line options are specified in a single argument:

- `-n`: Disable using the `cache` directory.
- `-a`: Do not redirect to `index.html`, or `index.php` when present.
- `-e`: Disable error pages.
