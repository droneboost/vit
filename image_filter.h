#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

namespace vit {

template <typename Image, typename Param, typename Ret>
class ImageFilter
{
public:
    ImageFilter(Image& i, Param& p)
        : img(i),
          param(p)
    {}
    virtual ~ImageFilter() {}

    virtual Ret applyToImage(Image& img, const Param& param) = 0; // Update both image and parameters
    virtual Ret applyToImage(const Param& param) = 0;             // Update parameters
    virtual Ret applyToImage(Image& img) = 0;                     // Update image

    Image& getImage() { return img; }
    Image& setImage(Image& i) { img = i; }
    Param& getParam() { return param; }
    Param& setParam(const Param& p) { param = p; }

private:
    ImageFilter();

    Image& img;
    Param& param;
};

} // namespace vit

#endif // IMAGE_FILTER_H
