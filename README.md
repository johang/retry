# retry

## What is this?

With *retry*, you can rerun a command until it succeeds.

## Example usage

    $ retry 2 wget http://example.com

## Dependencies (on Linux)

No dependencies.

## Building from git on a recent Debian/Ubuntu

    $ sudo apt-get install build-essential meson
    $ git clone https://github.com/johang/retry.git retry
    $ cd retry
    $ meson build
    $ cd build
    $ ninja
