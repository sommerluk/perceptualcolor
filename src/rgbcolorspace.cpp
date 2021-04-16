// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2020 Lukas Sommer sommerluk@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "perceptualcolorlib_internal.h"

// Own headers
// First the interface, which forces the header to be self-contained.
#include "PerceptualColor/rgbcolorspace.h"
// Second, the private implementation.
#include "rgbcolorspace_p.h"

#include "helper.h"

#include "polarpointf.h"

#include <QDebug>

namespace PerceptualColor
{
/** @brief Default constructor
 *
 * Creates an sRGB color space.
 */
RgbColorSpace::RgbColorSpace(QObject *parent)
    : QObject(parent)
    , d_pointer(new RgbColorSpacePrivate(this))
{
    // TODO The creation of profiles and transforms might fail!
    // TODO How to handle that?

    // Create an ICC v4 profile object for the Lab color space.
    cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(nullptr // nullptr means: Default white point (D50)
                                                                // TODO Does this make sense? sRGB white point is D65!
    );
    // Create an ICC profile object for the sRGB color space.
    cmsHPROFILE rgbProfileHandle = cmsCreate_sRGBProfile();
    d_pointer->m_cmsInfoDescription = d_pointer->getInformationFromProfile(rgbProfileHandle, cmsInfoDescription);
    d_pointer->m_cmsInfoCopyright = d_pointer->getInformationFromProfile(rgbProfileHandle, cmsInfoCopyright);
    d_pointer->m_cmsInfoManufacturer = d_pointer->getInformationFromProfile(rgbProfileHandle, cmsInfoManufacturer);
    d_pointer->m_cmsInfoModel = d_pointer->getInformationFromProfile(rgbProfileHandle, cmsInfoModel);
    // TODO Only change the description to "sRGB" if the build-in sRGB is
    // used, not when an actual external ICC profile is used.
    //    m_cmsInfoDescription = tr("sRGB"); // TODO ???
    // Create the transforms
    // We use the flag cmsFLAGS_NOCACHE which disables the 1-pixel-cache
    // which is normally used in the transforms. We do this because transforms
    // that use the 1-pixel-cache are not thread-save. And disabling it
    // should not have negative impacts as we usually work with gradients,
    // so anyway it is not likely to have two consecutive pixels with
    // the same color, which is the only situation where the 1-pixel-cache
    // makes processing faster.
    d_pointer->m_transformLabToRgbHandle = cmsCreateTransform(labProfileHandle,             // input profile handle
                                                              TYPE_Lab_DBL,                 // input buffer format
                                                              rgbProfileHandle,             // output profile handle
                                                              TYPE_RGB_DBL,                 // output buffer format
                                                              INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
                                                              cmsFLAGS_NOCACHE              // flags
    );
    d_pointer->m_transformLabToRgb16Handle = cmsCreateTransform(labProfileHandle,             // input profile handle
                                                                TYPE_Lab_DBL,                 // input buffer format
                                                                rgbProfileHandle,             // output profile handle
                                                                TYPE_RGB_16,                  // output buffer format
                                                                INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
                                                                cmsFLAGS_NOCACHE              // flags
    );
    d_pointer->m_transformRgbToLabHandle = cmsCreateTransform(rgbProfileHandle,             // input profile handle
                                                              TYPE_RGB_DBL,                 // input buffer format
                                                              labProfileHandle,             // output profile handle
                                                              TYPE_Lab_DBL,                 // output buffer format
                                                              INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
                                                              cmsFLAGS_NOCACHE              // flags
    );
    // Close profile (free memory)
    cmsCloseProfile(labProfileHandle);
    cmsCloseProfile(rgbProfileHandle);

    // Now we know for sure that lowerChroma is in-gamut and upperChroma is
    // out-of-gamut…
    LchDouble candidate;
    candidate.l = 0;
    candidate.c = 0;
    candidate.h = 0;
    while (!inGamut(candidate) && (candidate.l < 100)) {
        candidate.l += gamutPrecision;
    }
    d_pointer->m_blackpointL = candidate.l;
    candidate.l = 100;
    while (!inGamut(candidate) && (candidate.l > 0)) {
        candidate.l -= gamutPrecision;
    }
    d_pointer->m_whitepointL = candidate.l;
    if (d_pointer->m_whitepointL <= d_pointer->m_blackpointL) {
        qCritical() << "Unable to find blackpoint and whitepoint on gray axis.";
        throw 0;
    }
}

/** @brief Destructor */
RgbColorSpace::~RgbColorSpace() noexcept
{
    cmsDeleteTransform(d_pointer->m_transformLabToRgb16Handle);
    cmsDeleteTransform(d_pointer->m_transformLabToRgbHandle);
    cmsDeleteTransform(d_pointer->m_transformRgbToLabHandle);
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
RgbColorSpace::RgbColorSpacePrivate::RgbColorSpacePrivate(RgbColorSpace *backLink)
    : q_pointer(backLink)
{
}

/** @brief The darkest in-gamut point on the L* axis.
 *
 * @sa whitepointL */
qreal RgbColorSpace::blackpointL() const
{
    return d_pointer->m_blackpointL;
}

/** @brief The lightest in-gamut point on the L* axis.
 *
 * @sa blackpointL() */
qreal RgbColorSpace::whitepointL() const
{
    return d_pointer->m_whitepointL;
}

/** @brief Calculates the Lab value
 *
 * @param rgbColor the color that will be converted. (If this is not an
 * RGB color, it will be converted first into an RGB color by QColor methods.)
 * @returns If the color is valid, the corresponding LCh value might also
 * be invalid.
 */
cmsCIELab RgbColorSpace::colorLab(const QColor &rgbColor) const
{
    RgbDouble my_rgb;
    my_rgb.red = rgbColor.redF();
    my_rgb.green = rgbColor.greenF();
    my_rgb.blue = rgbColor.blueF();
    return d_pointer->colorLab(my_rgb);
}

PerceptualColor::LchDouble RgbColorSpace::colorLch(const QColor &rgbColor) const
{
    cmsCIELab lab = colorLab(rgbColor);
    cmsCIELCh lch;
    cmsLab2LCh(&lch, &lab);
    LchDouble result;
    result.l = lch.L;
    result.c = lch.C;
    result.h = lch.h;
    return result;
}

/** @brief Calculates the Lab value
 *
 * @param rgb the color that will be converted.
 * @returns If the color is valid, the corresponding LCh value might also
 * be invalid. */
cmsCIELab RgbColorSpace::RgbColorSpacePrivate::colorLab(const RgbDouble &rgb) const
{
    cmsCIELab lab;
    cmsDoTransform(m_transformRgbToLabHandle, // handle to transform function
                   &rgb,                      // input
                   &lab,                      // output
                   1                          // convert exactly 1 value
    );
    return lab;
}

/** @brief Calculates the RGB value
 *
 * @param Lab a L*a*b* color
 * @returns If the color is within the RGB gamut, a QColor with the RGB values.
 * An invalid QColor otherwise.
 */
QColor RgbColorSpace::colorRgb(const cmsCIELab &Lab) const
{
    QColor temp; // By default, without initialization this is an invalid color
    RgbDouble rgb;
    cmsDoTransform(d_pointer->m_transformLabToRgbHandle, // handle to transform function
                   &Lab,                                 // input
                   &rgb,                                 // output
                   1                                     // convert exactly 1 value
    );
    if (inRange<cmsFloat64Number>(0, rgb.red, 1) && inRange<cmsFloat64Number>(0, rgb.green, 1) && inRange<cmsFloat64Number>(0, rgb.blue, 1)) {
        // We are within the gamut
        temp = QColor::fromRgbF(rgb.red, rgb.green, rgb.blue);
    }
    return temp;
}

/** @brief Calculates the RGB value
 *
 * @param lch an LCh color
 * @returns If the color is within the RGB gamut, a QColor with the RGB values.
 * An invalid QColor otherwise.
 */
QColor RgbColorSpace::colorRgb(const LchDouble &lch) const
{
    cmsCIELab lab; // uses cmsFloat64Number internally
    const cmsCIELCh myCmsCieLch = toCmsCieLch(lch);
    // convert from LCh to Lab
    cmsLCh2Lab(&lab, &myCmsCieLch);
    cmsCIELab temp;
    temp.L = lab.L;
    temp.a = lab.a;
    temp.b = lab.b;
    return colorRgb(temp);
}

RgbDouble RgbColorSpace::RgbColorSpacePrivate::colorRgbBoundSimple(const cmsCIELab &Lab) const
{
    cmsUInt16Number rgb_int[3];
    cmsDoTransform(m_transformLabToRgb16Handle, // handle to transform function
                   &Lab,                        // input
                   rgb_int,                     // output
                   1                            // convert exactly 1 value
    );
    RgbDouble temp;
    temp.red = rgb_int[0] / static_cast<qreal>(65535);
    temp.green = rgb_int[1] / static_cast<qreal>(65535);
    temp.blue = rgb_int[2] / static_cast<qreal>(65535);
    return temp;
}

/** @brief Calculates the RGB value
 *
 * @param Lab a L*a*b* color
 * @returns If the color is within the RGB gamut, a QColor with the RGB values.
 * A nearby (in-gamut) RGB QColor otherwise.
 */
QColor RgbColorSpace::colorRgbBound(const cmsCIELab &Lab) const
{
    RgbDouble temp = d_pointer->colorRgbBoundSimple(Lab);
    return QColor::fromRgbF(temp.red, temp.green, temp.blue);
}

/** @brief Calculates the RGB value
 *
 * @param lch an LCh color
 * @returns If the color is within the RGB gamut, a QColor with the RGB values.
 * A nearby (in-gamut) RGB QColor otherwise.
 */
QColor RgbColorSpace::colorRgbBound(const LchDouble &lch) const
{
    cmsCIELab lab; // uses cmsFloat64Number internally
    const cmsCIELCh myCmsCieLch = toCmsCieLch(lch);
    // convert from LCh to Lab
    cmsLCh2Lab(&lab, &myCmsCieLch);
    cmsCIELab temp;
    temp.L = lab.L;
    temp.a = lab.a;
    temp.b = lab.b;
    return colorRgbBound(temp);
}

QColor RgbColorSpace::colorRgbBound(const PerceptualColor::LchaDouble &lcha) const
{
    LchDouble lch;
    lch.l = lcha.l;
    lch.c = lcha.c;
    lch.h = lcha.h;
    QColor result = colorRgbBound(lch);
    result.setAlphaF(lcha.a);
    return result;
}

// TODO What to do with in-gamut tests if LittleCMS has fallen back to
// bounded mode because of too complicate profiles? Out in-gamut detection
// would not work anymore!

/** @brief check if an LCh value is within a specific RGB gamut
 * @param lightness The lightness value
 * @param chroma The chroma value
 * @param hue The hue value (angle in degree)
 * @returns Returns true if lightness/chroma/hue is in the specified
 * RGB gamut. Returns false otherwise. */
bool RgbColorSpace::inGamut(const double lightness, const double chroma, const double hue) const
{
    // variables
    LchDouble LCh;

    // code
    LCh.l = lightness;
    LCh.c = chroma;
    LCh.h = hue;
    return inGamut(LCh);
}

/** @brief check if an LCh value is within a specific RGB gamut
 * @param lch the LCh color
 * @returns Returns true if lightness/chroma/hue is in the specified
 * RGB gamut. Returns false otherwise. */
bool RgbColorSpace::inGamut(const LchDouble &lch) const
{
    // variables
    cmsCIELab lab; // uses cmsFloat64Number internally
    const cmsCIELCh myCmsCieLch = toCmsCieLch(lch);

    // code
    cmsLCh2Lab(&lab,
               &myCmsCieLch); // TODO no normalization necessary previously?
    cmsCIELab temp;
    temp.L = lab.L;
    temp.a = lab.a;
    temp.b = lab.b;
    return inGamut(temp);
}

/** @brief check if a Lab value is within a specific RGB gamut
 * @param lab the Lab color
 * @returns Returns true if it is in the specified RGB gamut. Returns
 * false otherwise. */
bool RgbColorSpace::inGamut(const cmsCIELab &lab) const
{
    RgbDouble rgb;

    cmsDoTransform(d_pointer->m_transformLabToRgbHandle, // handle to transform function
                   &lab,                                 // input
                   &rgb,                                 // output
                   1                                     // convert exactly 1 value
    );

    return (inRange<cmsFloat64Number>(0, rgb.red, 1) && inRange<cmsFloat64Number>(0, rgb.green, 1) && inRange<cmsFloat64Number>(0, rgb.blue, 1));
}

QString RgbColorSpace::profileInfoCopyright() const
{
    return d_pointer->m_cmsInfoCopyright;
}

/** Returns the description of the RGB color space. */
QString RgbColorSpace::profileInfoDescription() const
{
    return d_pointer->m_cmsInfoDescription;
}

QString RgbColorSpace::profileInfoManufacturer() const
{
    return d_pointer->m_cmsInfoManufacturer;
}

/** @brief Convert to LCh
 *
 * @param lab a point in Lab representation
 * @returns the same point in LCh representation */
PerceptualColor::LchDouble RgbColorSpace::toLch(const cmsCIELab &lab) const
{
    cmsCIELCh tempLch;
    cmsLab2LCh(&tempLch, &lab);
    return toLchDouble(tempLch);
}

QString RgbColorSpace::profileInfoModel() const
{
    return d_pointer->m_cmsInfoModel;
}

/** @returns A <em>normalized</em> (this is guaranteed!) in-gamut color,
 * maybe with different chroma (and even lightness??) */
PerceptualColor::LchDouble RgbColorSpace::nearestInGamutSacrifyingChroma(const PerceptualColor::LchDouble &color) const
{
    LchDouble result = color;
    PolarPointF temp(result.c, result.h);
    result.c = temp.radial();
    result.h = temp.angleDegree();

    // Test special case: If we are yet in-gamut…
    if (inGamut(result)) {
        return result;
    }

    // Now we know: We are out-of-gamut…
    LchDouble lowerChroma {result.l, 0, result.h};
    LchDouble upperChroma {result};
    LchDouble candidate;
    if (inGamut(lowerChroma)) {
        // Now we know for sure that lowerChroma is in-gamut
        // and upperChroma is out-of-gamut…
        candidate = upperChroma;
        while (upperChroma.c - lowerChroma.c > gamutPrecision) {
            // Our test candidate is half the way between lowerChroma
            // and upperChroma:
            candidate.c = ((lowerChroma.c + upperChroma.c) / 2);
            if (inGamut(candidate)) {
                lowerChroma = candidate;
            } else {
                upperChroma = candidate;
            }
        }
        result = lowerChroma;
    } else {
        if (result.l < blackpointL()) {
            result.l = blackpointL();
            result.c = 0;
        } else {
            if (result.l > whitepointL()) {
                result.l = blackpointL();
                result.c = 0;
            }
        }
    }

    return result;
}

/** @brief Get information from an ICC profile via LittleCMS
 *
 * @param profileHandle handle to the ICC profile in which will be searched
 * @param infoType the type of information that is searched
 * @returns A QString with the information. First, it searches the
 * information in the current locale (language code and country code as
 * provided currently by <tt>QLocale</tt>). If the information is not
 * available in this locale, it silently falls back to another available
 * localization. Note that the returned QString() might be empty if the
 * requested information is not available in the ICC profile. */
QString RgbColorSpace::RgbColorSpacePrivate::getInformationFromProfile(cmsHPROFILE profileHandle, cmsInfoType infoType)
{
    // Initialize a char array of 3 values (two for actual characters and a
    // one for a terminating null)cmsFloat64Number
    // The recommended default value for language
    // following LittleCMS documentation is “en”.
    char languageCode[3] = "en";
    // The recommended default value for country
    // following LittleCMS documentation is “US”.
    char countryCode[3] = "US";
    // Update languageCode and countryCode to the actual locale (if possible)
    const QStringList list = QLocale().name().split(QStringLiteral(u"_"));
    // The locale codes should be ASCII only, so QString::toUtf8() should
    // return ASCII-only valid results. We do not know what character encoding
    // LittleCMS expects, but ASCII seems a save choise.
    const QByteArray currentLocaleLanguage = list.at(0).toUtf8();
    const QByteArray currentLocaleCountry = list.at(1).toUtf8();
    if (currentLocaleLanguage.size() == 2) {
        languageCode[0] = currentLocaleLanguage.at(0);
        languageCode[1] = currentLocaleLanguage.at(1);
        // No need for languageCode[2] = 0; for null-terminated string,
        // because the array was yet initialized
        if (currentLocaleCountry.size() == 2) {
            countryCode[0] = currentLocaleCountry.at(0);
            countryCode[1] = currentLocaleCountry.at(1);
            // No need for countryCode[2] = 0; for null-terminated string,
            // because the array was yet initialized
        }
    }
    // Calculate the size of the buffer that we have to provide for
    // cmsGetProfileInfo in order to return a value.
    const cmsUInt32Number resultLength = cmsGetProfileInfo(
        // Profile in which we search:
        profileHandle,
        // The type of information we search:
        infoType,
        // The preferred language in which we want to get the information:
        languageCode,
        // The preferred country for which we want to get the information:
        countryCode,
        // Do not actually provide the information,
        // just return the required buffer size:
        nullptr,
        // Do not actually provide the information,
        // just return the required buffer size:
        0);
    // For the actual buffer size, increment by 1. This helps us to
    // guarantee a null-terminated string later on.
    const cmsUInt32Number bufferLength = resultLength + 1;

    // Allocate the buffer
    wchar_t *buffer = new wchar_t[bufferLength];
    // Initialize the buffer with 0
    for (cmsUInt32Number i = 0; i < bufferLength; ++i) {
        *(buffer + i) = 0;
    }

    // Write the actual information to the buffer
    cmsGetProfileInfo(
        // profile in which we search
        profileHandle,
        // the type of information we search
        infoType,
        // the preferred language in which we want to get the information
        languageCode,
        // the preferred country for which we want to get the information
        countryCode,
        // the buffer into which the requested information will be written
        buffer,
        // the buffer size as previously calculated
        resultLength);
    // Make absolutely sure the buffer is null-terminated by marking its last
    // element (the one that was the +1 "extra" element) as null.
    *(buffer + (bufferLength - 1)) = 0;

    // Create a QString() from the from the buffer
    // cmsGetProfileInfo returns often strings that are smaller than the
    // previously calculated buffer size. But it seems they are
    // null-terminated strings. So we read only up to the first null value.
    // This is save, because we made sure previously that the buffer is
    // indeed null-terminated.
    // QString::fromWCharArray will return a QString. It accepts arrays of
    // wchar_t. wchar_t might have different sizes, either 16 bit or 32 bit
    // depending on the operating system. As Qt’s documantation of
    // QString::fromWCharArray() says:
    //
    //     “If wchar is 4 bytes, the string is interpreted as UCS-4,
    //      if wchar is 2 bytes it is interpreted as UTF-16.”
    //
    // However, apparently this is not exact: When wchar is 4 bytes,
    // surrogate pairs in the code unit array are interpretated like
    // UTF-16: The surrogate pair is recognized as such. TODO static_assert that
    // this is true (which seems complicate: better provide a unit test!) (Which
    // is not strictly UTF-32 conform). Single surrogates cannot be
    // interpretated correctly, but there will be no crash:
    // QString::fromWCharArray will continue to read, also the part after
    // the first UTF error. We can rely on this behaviour: As we do
    // not really know the encoding of the buffer that LittleCMS returns.
    // Therefore, it is a good idea to be compatible for various
    // interpretations.
    QString result = QString::fromWCharArray(buffer, // read from this buffer
                                             -1      // read until the first null element
    );

    // Free allocated memory of the buffer
    delete[] buffer;

    // Return
    return result;
}

}
