# Tiger
A really small and fast (like a tiger) nomad web server written in C, that can be installed in any directory and transferred anywhere on the file system.

## Getting Started
First download the source code and compile it.

```bash
git clone https://github.com/kevidryon2/tiger.git
cd tiger
mkdir build
make
```

Since Tiger only consists of less than 1K lines of code, it is really fast to compile; by default, it will compile for the x86 architecture, but if you want to compile for another architecture (ARM32, ARM64, or Risc-V) just specify one of the following arguments to make: `arm`, `arm64`, `riscv`.

If you want to compile multiple architectures at once, choose between the following arguments:
`x86-arch`, `arm-arch`, `aarch64`, `riscv-arch`.

There are a few flags you can define to alter the compilation of Tiger that you can put in CFLAGS:

- `-DDISABLE_CACHE`: Disable caching; this is particularly useful for web developers who want to test their website using Tiger without having to constantly clear the cache.
- `-DNO-REDIRECT-ROOT`: Disable redirecting `/` to `/index.html`.
- `-DREDIRECT-ROOT-PHP`: Redirect `/` to `/index.php`.
- `-DERRORS-PHP`: Set the default error pages to `/(error).php` (For example, `/404.php` or `/500.php`)

-

Then, create Tiger's directory structure and copy Tiger to it; in this example, we'll be using the `/srv` directory, but you can use any directory you want.

```bash
mkdir --parents /srv/public /srv/cache /srv/scripts /srv/bin
cp build/tiger /src/bin
```

-

Lastly, start Tiger.

```
cd /srv/bin
export TIGER_PATH=..
./tiger
Tiger Beveren 1 (PID = <some random number>)
Using port 8080

Creating socket... OK
Binding... OK
Listening... OK

Using directory /srv/

Loading scripts...
Loaded Tiger. Accepting requests.
```
