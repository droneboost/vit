#include "opencv2/core/mat.hpp"
#include "opencv2/imgproc.hpp"

#include "image_filter_ade.h"

namespace vit {

ImageFilterADE::ImageFilterADE(cv::Mat &mat, ADEParam &param)
    : ImageFilter(mat, param)
{
}

ImageFilterADE::~ImageFilterADE()
{
}

ADERet ImageFilterADE::applyToImage(cv::Mat& mat, const ADEParam &param)
{
    cv::Mat contrBright = cv::Mat::zeros(mat.size(), mat.type());
    // qDebug("mat type=%d, CV_16UC1=%d\n", mat.type(), CV_16UC1);

    // contrast and brightness
    for (int y = 0; y < mat.rows; ++y) {
        for (int x = 0; x < mat.cols; ++x) {
            contrBright.at<ushort>(y, x) =
                    cv::saturate_cast<ushort>(param.alpha * (mat.at<ushort>(y,x)) + param.beta);
        }
    }

    // sharpen
    cv::Mat gaussBlur = cv::Mat::zeros(contrBright.size(), contrBright.type());
    cv::GaussianBlur(contrBright, gaussBlur, cv::Size(0, 0), param.sigmaX, param.sigmaY);
    double threshold = param.threshold;
    double amount = param.amount;
    cv::Mat lowContrMask = cv::abs(contrBright - gaussBlur) < threshold;
    mat = contrBright * (1 + amount) + gaussBlur * (-amount);
    gaussBlur.copyTo(mat, lowContrMask);

    // keep reference of image and params
    ImageFilter::setImage(mat);
    ImageFilter::setParam(param);

    return ADE_RESULT_OK;
}

ADERet ImageFilterADE::applyToImage(const ADEParam &param)
{
    return applyToImage(ImageFilter::getImage(), param);
}

ADERet ImageFilterADE::applyToImage(cv::Mat& mat)
{
    return applyToImage(mat, ImageFilter::getParam());
}

} // namespace vit
