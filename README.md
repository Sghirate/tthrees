# tthrees

Terminal version of [Threes](http://asherv.com/threes/), to try out [imtui](https://github.com/ggerganov/imtui).

## Building

**Linux**:

Prerequisites:
* CMake (>= 3.10)
* A C++ compiler (for example GCC 7.5.0)
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

## Third Party SDKs

|Software                                                |License     |Comment                     |
|--------------------------------------------------------|------------|----------------------------|
|[imtui](https://github.com/ggerganov/imtui)             |MIT         |UI library, terminal backend|
|[imgui](https://github.com/ocornut/imgui)               |MIT         |UI library, frontend        |
|[mini-yaml](https://github.com/jimmiebergmann/mini-yaml)|MIT         |YAML parser                 |
|[google-test](https://github.com/google/googletest)     |BSD-3-Clause| Used by mini-yaml          |

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