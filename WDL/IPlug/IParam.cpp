#include "IParam.h"
#include <stdio.h>
//#include <iostream>

IParam::IParam()
  : mType(kTypeNone)
  , mValue(0.0)
  , mMin(0.0)
  , mMax(1.0)
  , mStep(1.0)
  , mDisplayPrecision(0)
  , mNegateDisplay(false)
  , mShape(1.0)
  , mCanAutomate(true)
  , mDefault(0.)
  , mIsMeta(false)
{
  memset(mName, 0, MAX_PARAM_NAME_LEN * sizeof(char));
  memset(mLabel, 0, MAX_PARAM_LABEL_LEN * sizeof(char));
  memset(mParamGroup, 0, MAX_PARAM_LABEL_LEN * sizeof(char));
}

IParam::~IParam() {}

void IParam::InitBool(const char* name, bool defaultVal, const char* label, const char* group)
{
  if (mType == kTypeNone) mType = kTypeBool;
  
  InitEnum(name, (defaultVal ? 1 : 0), 2, label, group);
}

void IParam::InitEnum(const char* name, int defaultVal, int nEnums, const char* label, const char* group)
{
  if (mType == kTypeNone) mType = kTypeEnum;
  
  InitInt(name, defaultVal, 0, nEnums - 1, label, group);
}

void IParam::InitInt(const char* name, int defaultVal, int minVal, int maxVal, const char* label, const char* group)
{
  if (mType == kTypeNone) mType = kTypeInt;
  
  InitDouble(name, (double) defaultVal, (double) minVal, (double) maxVal, 1.0, label, group);
}

void IParam::InitDouble(const char* name, double defaultVal, double minVal, double maxVal, double step, const char* label, const char* group, double shape)
{
  if (mType == kTypeNone) mType = kTypeDouble;
  
  strcpy(mName, name);
  strcpy(mLabel, label);
  strcpy(mParamGroup, group);
  mValue = defaultVal;
  mMin = minVal;
  mMax = IPMAX(maxVal, minVal + step);
  mStep = step;
  mDefault = defaultVal;

  for (mDisplayPrecision = 0;
       mDisplayPrecision < MAX_PARAM_DISPLAY_PRECISION && step != floor(step);
       ++mDisplayPrecision, step *= 10.0)
  {
    ;
  }
  
  SetShape(shape);
}

void IParam::SetShape(double shape)
{
  if(shape != 0.0)
    mShape = shape;
}

void IParam::SetDisplayText(int value, const char* text)
{
  int n = mDisplayTexts.GetSize();
  mDisplayTexts.Resize(n + 1);
  DisplayText* pDT = mDisplayTexts.Get() + n;
  pDT->mValue = value;
  strcpy(pDT->mText, text);
}

double IParam::DBToAmp()
{
  return ::DBToAmp(mValue);
}

void IParam::SetNormalized(double normalizedValue)
{
  mValue = FromNormalizedParam(normalizedValue, mMin, mMax, mShape);
  
  if (mType != kTypeDouble)
  {
    mValue = floor(0.5 + mValue / mStep) * mStep;
  }
  
  mValue = IPMIN(mValue, mMax);
}

double IParam::GetNormalized()
{
  return GetNormalized(mValue);
}

double IParam::GetNormalized(double nonNormalizedValue)
{
  nonNormalizedValue = BOUNDED(nonNormalizedValue, mMin, mMax);
  return ToNormalizedParam(nonNormalizedValue, mMin, mMax, mShape);
}

//--------------------------- [GN] ------------------------------------//
// Not happy about all this, should be in a subclass, mainly for CP1A

double Db2lin(double dB ) {
	return exp(dB * 0.11512925464970228420089957273422);
}

double Lin2db(double lin) {
	return log(lin) * 8.6858896380650365530225783783321;
}

// -100 to +6 in linear
double mixdb(double value) {
	if (value <  0.5f) {
        value = -100.f + (100.f * (value * 2.0f));
    } else if (value == 0.5f) {
        value = -100.f + (100.f * (value * 2.0f));
    } else if (value >  0.5f) {
        value = -6.f  + (value * 12.0f);
    }
	return Db2lin(value);
}

// -80 - 0 - +6: convert to db for display
void MixDbDisplay(char *text, double value) {
   
	double temp = Lin2db(mixdb(value));
	if (temp < -99) {
		sprintf(text,"inf");
	} else {
		sprintf(text,"%2.1f", temp);
	}
}

double increaseLowResolution(double value) {
	return (2.175345 * (1.0 - cos(value)));
}

//------------------------------------------------------------------//

void IParam::GetDisplayForHost(double value, bool normalized, char* rDisplay, bool withDisplayText)
{

  if (normalized) value = FromNormalizedParam(value, mMin, mMax, mShape);

  /// [GN] START ///
    
  if (MixDb) {
      char displayText[64];
      MixDbDisplay(displayText, value);
      if (CSTR_NOT_EMPTY(displayText)) {
          strcpy(rDisplay, displayText);
      }
      return;
  }
  /// [GN] END ///
    
  if (withDisplayText)
  {
    unsigned int numTextElements = GetNDisplayTexts();
    unsigned int textIndex = (unsigned int)floor(value * numTextElements);
    if (textIndex == numTextElements) {
        textIndex = numTextElements - 1;
    }
    const char* displayText = GetDisplayText(textIndex);
    // std::cout << mName << " textIndex:" << textIndex << " = (" << value << " * " << GetNDisplayTexts() << ")" << std::endl;
    if (CSTR_NOT_EMPTY(displayText))
    {
      strcpy(rDisplay, displayText);
      return;
    }
  }
    
  double displayValue = value;

  if (mNegateDisplay) displayValue = -displayValue;

  if (mDisplayPrecision == 0)
  {
    sprintf(rDisplay, "%d", int(displayValue));
  }
//   else if(mSignDisplay)
//   {
//     char fmt[16];
//     sprintf(fmt, "%%+.%df", mDisplayPrecision);
//     sprintf(rDisplay, fmt, displayValue);
//   }
  else
  {
    sprintf(rDisplay, "%.*f", mDisplayPrecision, displayValue);
  }
}

const char* IParam::GetNameForHost()
{
  return mName;
}

const char* IParam::GetLabelForHost()
{
  const char* displayText = GetDisplayText((int) mValue);
  return (CSTR_NOT_EMPTY(displayText)) ? "" : mLabel;
}

const char* IParam::GetParamGroupForHost()
{
  return mParamGroup;
}

int IParam::GetNDisplayTexts()
{
  return mDisplayTexts.GetSize();
}

const char* IParam::GetDisplayText(int value)
{
  int n = mDisplayTexts.GetSize();
  if (n)
  {
    DisplayText* pDT = mDisplayTexts.Get();
    for (int i = 0; i < n; ++i, ++pDT)
    {
      if (value == pDT->mValue)
      {
        return pDT->mText;
      }
    }
  }
  return "";
}

const char* IParam::GetDisplayTextAtIdx(int idx, int* value)
{
  DisplayText* pDT = mDisplayTexts.Get()+idx;
  
  if (value) 
  {
    *value = pDT->mValue;
  }
  return pDT->mText;
}

bool IParam::MapDisplayText(char* str, int* pValue)
{
  int n = mDisplayTexts.GetSize();
  
  if (n)
  {
    DisplayText* pDT = mDisplayTexts.Get();
    for (int i = 0; i < n; ++i, ++pDT)
    {
      if (!strcmp(str, pDT->mText))
      {
        *pValue = pDT->mValue;
        return true;
      }
    }
  }
  return false;
}

void IParam::GetBounds(double* pMin, double* pMax)
{
  *pMin = mMin;
  *pMax = mMax;
}