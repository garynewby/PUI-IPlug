
// puiTest_Effect.h


#ifndef __puiTest_Effect__
#define __puiTest_Effect__

#include <math.h>


enum {
	kIn,
	kOut,
	kBypass,
	kNumParams,
	kNumPrograms = 1,
	kInputCount  = 2,
	kOutputCount = 2,
	kinfo = 100
};


class puiTest_Effect {
public:
	
    puiTest_Effect(double sr);
	~puiTest_Effect();
	
    void set (int index, double value, double sr);
	void process_stereo (double **inputs, double **outputs, int sampleFrames);
	void process_mono (double **inputs, double **outputs, int sampleFrames);
	void setSR(double SR);
	double getSR();
	bool Bypassed();
	
protected:
	
	double sampleRate;

};

#endif





