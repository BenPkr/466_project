#include "systemc.h"
#include "digit.h"

#ifndef _MODULES_H_
#define _MODULES_H_ 1

SC_MODULE (splitter)
{
    sc_in <NN_DIGIT> in_data;
    sc_out <NN_DIGIT> out_data_h, out_data_l;
    
    void run_splitter();
    
    SC_CTOR(splitter) {
        SC_METHOD(run_splitter);
        sensitive << in_data;
    }
};

SC_MODULE (multiplier)
{
	sc_in<NN_DIGIT> in_data_1;
	sc_in<NN_DIGIT> in_data_2;
	sc_out<NN_DIGIT> out_data;

	void run_multiplier();

	SC_CTOR (multiplier)
	{
		SC_METHOD(run_multiplier);
		sensitive << in_data_1 << in_data_2;
	}
};

SC_MODULE (multiplexor)
{
	sc_in<NN_DIGIT> in_data_1;
	sc_in<NN_DIGIT> in_data_2;
	sc_in<bool> select;
	sc_out<NN_DIGIT> out_data;

	void run_multiplexor();

	SC_CTOR (multiplexor)
	{
		SC_METHOD(run_multiplexor);
		sensitive << in_data_1 << in_data_2 << select;
	}
};

SC_MODULE (comparator)
{
	sc_in<NN_DIGIT> in_data_1;
	sc_in<NN_DIGIT> in_data_2;
	sc_out<bool> less_than_signal;

	void run_comparator();

	SC_CTOR (comparator)
	{
		SC_METHOD(run_comparator);
		sensitive << in_data_1 << in_data_2;
	}
};

SC_MODULE (subtractor)
{
    sc_in<NN_DIGIT> in_data_1;
    sc_in<NN_DIGIT> in_data_2;
    sc_out<NN_DIGIT> out_data;

    void run_subtractor();

    SC_CTOR (subtractor)
    {
        SC_METHOD(run_subtractor);
        sensitive << in_data_1 << in_data_2;
    }
};

SC_MODULE (bit_shift)
{
    sc_in<NN_DIGIT> in_data;
    sc_out<NN_DIGIT> out_data;

    void run_bit_shift();

    SC_CTOR (bit_shift) {
        SC_METHOD(run_bit_shift);
        sensitive << in_data;
    }
};


#endif /* end _MODULES_H_ */