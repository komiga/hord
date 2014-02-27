
# Hord hourd hoerd hoard

This here is a replacement framework for my log-keeping software. It
is of use to precisely one human. *Hord* is an Old English noun
equivalent to *hoard*. Fascinating, yes, I know.

## Dependencies

Hord uses:

1. [trait_wrangler](https://github.com/komiga/trait_wrangler) (HEAD)
2. [ceformat](https://github.com/komiga/ceformat) (HEAD)
3. [duct++](https://github.com/komiga/duct-cpp) (HEAD)
4. [murk](https://github.com/komiga/murk) (HEAD)

See `dep/README.md` for dependency setup.

## Building

Hord is a compiled shared library (by default).

All features support Clang 3.5+ with libc++ (SVN head). GCC is not supported
due to the ceformat dependency.

Once dependencies are setup (see `dep/README.md`), the library and tests can be
compiled using plash's standard project protocol: http://komiga.com/pp-cpp

## License

Hord carries the MIT license, which can be found below and in the `LICENSE`
file.

```
Copyright (c) 2013-2014 Tim Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
