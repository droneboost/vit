#ifndef IMAGE_FILTER_ADE_H
#define IMAGE_FILTER_ADE_H

#include "image_filter.h"

namespace vit {

typedef enum ADEResult
{
    ADE_RESULT_OK = 0,
    ADE_RESULT_NG
} ADERet;

struct ADEParam
{
    double alpha;      // Simple contrast control 1.0~8.0
    unsigned int beta; // Simple brightness control 0~20000

    double sigmaX;     // GaussianBlur sigmaX 0.0~6.0
    double sigmaY;     // GaussianBlur sigmaY 0.0~6.0

    double threshold;  // Sharpen threshold 0.0~1.0
    double amount;     // Sharpen amuont 1.0~5.0

    ADEParam()
        : alpha(6.5),
          beta(15000),
          sigmaX(4.5),
          sigmaY(4.5),
          threshold(1.0),
          amount(5.0)
    {}

    ADEParam(const double& a, const unsigned int& b,
             const double& sx, const double& sy,
             const double& t, const double& amt)
        : alpha(a),
          beta(b),
          sigmaX(sx),
          sigmaY(sy),
          threshold(t),
          amount(amt)
    {}

    bool operator==(const ADEParam& p) const
    {
        return ((alpha == p.alpha)
                && (beta == p.beta)
                && (sigmaX == p.sigmaX)
                && (sigmaY == p.sigmaY)
                && (threshold == p.threshold)
                && (amount == p.amount));
    }
};

class ImageFilterADE : public ImageFilter<cv::Mat, struct ADEParam, ADERet>
{
public:
    ImageFilterADE(cv::Mat& mat, ADEParam& param);

    virtual ~ImageFilterADE();

    virtual ADERet applyToImage(cv::Mat& mat, const ADEParam& param);
    virtual ADERet applyToImage(const ADEParam& param);
    virtual ADERet applyToImage(cv::Mat& mat);

private:
    ImageFilterADE();
};

} // namespace vit

#endif // IMAGE_FILTER_ADE_H
