# tthrees

Terminal version of [Threes](http://asherv.com/threes/), initially to try out [imtui](https://github.com/ggerganov/imtui) - however, by now everything related to it has been replaced by custom code (src/tui.hpp).

## Building

**Linux**:

Prerequisites:
* CMake (>= 3.10)
* A C++ compiler (tested with GCC/G++ 9.3.0 & Clang 10.0.0)
* ncurses (libncurses5-dev)

```bash
git clone https://github.com/Sghirate/tthrees.git
cd  tthrees
mkdir build
cd build
cmake ..
make
./bin/tthrees
```

**Windows**:
* CMake (>= 3.10)
* A C++ compiler (tested with MSVC 2019)

## License

Any project code (residing in /src/**) is licensed under the following license:

```
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 If you happen to meet one of the copyright holders in a bar you are obligated
 to buy them one pint of beer.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 ```
