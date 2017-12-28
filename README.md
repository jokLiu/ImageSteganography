# Image Steganography

Static C++ library implementing several different algorithms of image steganography.

Steganography is the practice of concealing a file, message, image, or video within another file, message, image, or video. 

## Motivation

In childhood I was amazed to come across the movie where some sort of simple steganography was used between prisoners etc. So... some time later I am wandering some side projects to work on and the picture of these "cool" tricks appear in my eyes, so I decide to dive in!!!

## Code Example

```c++
#include "steganography.h"
#include <iostream>

int main() {

  std::string str = "Lorem Ipsum is simply dummy text of the printing " \
  					"and typesetting industry. Lorem Ipsum has been "\
  					"the industry's standard dummy text ever since the "\
  					"1500s, when an unknown printer took a galley of " \
  					"type and scrambled it to make a type specimen book. "\
  					"It has survived not only five centuries, but also "\
  					"the leap into electronic typesetting, remaining "\
  					"essentially unchanged. It was popularised in the "\
  					"1960s with the release of Letraset sheets containing "\
  					"Lorem Ipsum passages, and more recently with desktop "\
  					"publishing software like Aldus PageMaker including "\
  					"versions of Lorem Ipsum.";

  steg::StegCoding::LSB_encode("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode("skull2.png") << "\n";

  steg::StegCoding::LSB_encode_odd("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode_odd("skull2.png") << "\n";

  steg::StegCoding::LSB_encode_even("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode_even("skull2.png") << "\n";

  steg::StegCoding::LSB_encode_prime("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode_prime("skull2.png") << "\n";

  steg::StegCoding::LSB_encode_max("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode_max("skull2.png") << "\n";
  
  steg::StegCoding::LSB_encode_min("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode_min("skull2.png") << "\n";
  
  steg::StegCoding::LSB_encode_spiral("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode_spiral("skull2.png") << "\n";

  steg::StegCoding::LSB_encode_magic_sq("skull2.png", str);

  std::cout <<steg::StegCoding::LSB_decode_magic_sq("skull2.png") << "\n";

  steg::StegCoding::encode_binary_image("city.png", "image_to_hide.pbm");

  steg::StegCoding::decode_binary_image("city.png", "hidden_image_to_store.pbm");
  
  return 0;
}
```



## API Reference

Ideas are taken from multiple article, one I would like to mention is ["A  New  Approach  in  Steganography  using  different Algorithms and Applying Randomization Concept"](https://www.ijarcce.com/upload/november/13-A%20New%20Approach%20in%20Steganography.pdf).

The library is implemented using [CImg Library](http://cimg.eu/) open source image processing library, therefore this dependency has to be included separately. It is very easy to use though.

#### Compiling

For compilation on different systems please refer to [Cimg Library](http://cimg.eu/reference/group__cimg__overview.html).

The generic way to compile on Linux can be achieved using the following command:

* `g++ -o main *.cpp -O2 -L/usr/X11R6/lib -std=c++11 -lm -lpthread -lX11 `

####Generic Function Summary

All the functions and detailed descriptions can be found in the `steganography.h`source file.

All the encode functions takes the following input:

* **name** - file/image name to be processed/encoded.
* **message** - the image to be hidden/encoded into the image (given by **name**)

``` c++
static void LSB_encode(const std::string &name, const std::string &message);
```

All the decode functions takes the following input:

- **name** - file/image name to be processed, from which message is retrieved.

```c++
static std::string LSB_decode(const std::string &name);
```

#### Functions

1. Simple LSB encode method encodes the message bit in every Least Significant Bit of each pixel, and the decode method is used to decode the image encoded with LSB_encode.

```c++
static void LSB_encode(const std::string &name, const std::string &message);
static std::string LSB_decode(const std::string &name);
```

2. Encodes the message bit in every odd location of the image using Least Significant Bit of each pixel, and the decode method is used to decode the image encoded with LSB_encode_odd.

```c++
static void LSB_encode_odd(const std::string& name, const std::string& message);
static std::string LSB_decode_odd(const std::string& name);
```

3. Encodes the message bit in every even location of the image using Least Significant Bit of each pixel, and the decode method is used to decode the image encoded with LSB_encode_even.

```c++
static void LSB_encode_even(const std::string& name, const std::string& message);
static std::string LSB_decode_even(const std::string& name);
```

4. Encodes the message bit in every largest pixel of each row. The BLUE (out of RGB) value of each the pixel is encoded while the largest value is checked against RED value. The decode method is used to decode the image encoded with LSB_encode._max.

```c++
static void LSB_encode_max(const std::string& name, const std::string& message);
static std::string LSB_decode_max(const std::string& name);
```

5. Encodes the message bit in every smallest pixel of each row. The BLUE (out of RGB) value of each the pixel is encoded while the smallest value is checked against RED value. The decode method is used to decode the image encoded with LSB_encode._min.

```c++
static void LSB_encode_min(const std::string &name, const std::string &message);
static std::string LSB_decode_min(const std::string &name);
```

6. Encodes the message bit in every prime pixel (in every pixel in prime location), and the decode method is used to decode the image encoded with LSB_encode_prime.

```c++
static void LSB_encode_prime(const std::string& name, const std::string& message);
static std::string LSB_decode_prime(const std::string& name);
```

7. Encodes the message bit in every prime spiral matrix diagonal location, the decode method is used to decode the image encoded with LSB_encode_spiral.

```c++
static void LSB_encode_spiral(const std::string& name, const std::string& message);
static std::string LSB_decode_spiral(const std::string& name);
```

8. Encodes the message bit in every magic square location, the decode method is used to decode the image encoded with LSB_encode_spiral.

```c++
static void LSB_encode_magic_sq(const std::string& name, const std::string& message);
static std::string LSB_decode_magic_sq(const std::string& name);
```

9. Encodes the binary pbm format image (**binary_image**) into another image provided by the **name**. At the moment simple LSB_encode/decode method is used, but will be possible to choose any method from the above.

```c++
static void encode_binary_image(const std::string& name, const std::string& binary_image);
static void decode_binary_image(const std::string& name, const std::string& binary_image);
```

**Explanation of the binary image encoding**

Imagine you are a secret agent who wants to transfer the nuclear warhead plans (image below) from country A to country B , however, country A can see all the outgoing and incoming traffic, therefore, you pretend to send a beautiful city image to your friend with a nuclear plans hidden inside the image. You can see that there is almost not difference between the stego image (encoded image) and uncoded one. 

(Note: later steg analysis will be implemented to show the differences between encoded images and uncoded ones. Also, we will be able to determine the best performed methods. )



**Nuclear Rocket image to be hidden**

![Nuclear rockets](https://github.com/jokLiu/ImageSteganography/blob/master/examples/images/nuclear.gif?raw=true)

**Uncoded image**![city](https://github.com/jokLiu/ImageSteganography/blob/master/examples/images/city_main.png?raw=true)



**Encoded image**

![city](https://github.com/jokLiu/ImageSteganography/blob/master/examples/images/city.png?raw=true)

## TODO List

* Fix `LSB_encode_magic_sq ` as some bits are destroyed. (issue: bad magic square implementation).
* Implementing function to hide binary images. (with the possibility to use any method).
* Adding additional parameters where to store encoded image because at the moment message is encoded into the same image.
* Adding warnings/checks to indicate that not all the message was encoded due to the capacity reasons.
* Creating some examples of possible usages.
* Parallel image processing.
* Implementing steg analysis. (possible methods for measurement:   Mean  Square  Error(MSE) , Peak  to  Signal  Noise  Ratio (PSNR), Structural SIMilarity  (SSIM) ).

## Contributing

Everyone is welcome to fork and issue a pull requests.

Note: Contributing.md will be available soon.

## License

Project is released under the [MIT License](https://jokliu.mit-license.org/).

## Author

[Jokubas Liutkus](https://github.com/jokLiu)