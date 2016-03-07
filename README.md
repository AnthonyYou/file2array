# file2array 小工具
将文件转换为数组的小工具。
windows中有资源文件的概念，但linux中没有，如果要将一个文件资源保存到可执行文件中，可以将文件转换为数组静态编译到可执行文件中，此工具支持输出c/c++头文件，使用方法如下：
```shell
$ ./file2array OPTIONS
  OPTIONS:
		-i input file
		-o output header file(without .h)
		-l language c or cpp
```

如有一个a.pdf文件，需要转成数组，则可以使用如下命令：
```shell
./file2array -i a.pdf -o pdf -l c
```
输出c语言的头文件格式，输出的文件名为pdf.h，文件内容为：
```c
#ifndef PDF_H
#define PDF_H
const unsigned char pdf_data[] = {0xaa, 0xbb, 0xcc......};
const long size = 3556;

#endif // end of PDF_DATA_H
```

pdf_data为文件转成的数组，size为数组的长度，也就是输入文件的长度。

也可以使用如下命令：
```shell
./file2array -o a.pdf -o pdf -l cpp
```
这种方式输出头文件格式如下：

```cpp
#ifndef PDF_H
#define PDF_H
namespace PDF {
		  const unsigned char data[] = {0xaa, 0xbb, 0xcc......};
		  const long size = 3556;
}

#endif // end of PDF_H
```

指定cpp格式输出，将数组包含在一个namespace中。

#这是一个qtcreator工程，但只包含一个main.cpp，可以不用这个工程直接编译main.cpp即可，另，因为使用了get_opt，所以只能在unix like系统上编译。
