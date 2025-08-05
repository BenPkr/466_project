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

SC_MODULE (reg)
{
	sc_in_clk clk;
	sc_in<NN_DIGIT> in_data;
	sc_in<bool> load_data;
	sc_out<NN_DIGIT> out_data;

	void run_reg();

	SC_CTOR (reg)
	{
		SC_CTHREAD(run_reg, clk.pos());
		out_data.initialize(0);
	}
};

SC_MODULE (half_reg)
{
	sc_in_clk clk;
	sc_in<NN_HALF_DIGIT> in_data;
	sc_in<bool> load_data;
	sc_out<NN_HALF_DIGIT> out_data;

	void run_reg();

	SC_CTOR (half_reg)
	{
		SC_CTHREAD(run_reg, clk.pos());
		out_data.initialize(0);
	}
};

SC_MODULE(zero_extender)
{
    sc_in<NN_HALF_DIGIT> in_data;
    sc_out<NN_DIGIT> out_data;

    void run_extender();

    SC_CTOR(zero_extender) {
        SC_METHOD(run_extender);
        sensitive << in_data;
    }
};

SC_MODULE(loop_engine) {
    
    sc_in_clk clk;
    sc_in<bool> start;
    sc_in<bool> reset;

    
    sc_in<NN_DIGIT> cHigh, cLow;
    sc_in<NN_DIGIT> t0_in, t1_in;
    sc_in<NN_HALF_DIGIT> aHigh_in;

    
    sc_out<NN_DIGIT> t0_out, t1_out;
    sc_out<NN_HALF_DIGIT> aHigh_out;
    sc_out<bool> done;

    void run();

    SC_CTOR(loop_engine) {
        SC_CTHREAD(run, clk.pos());
        reset_signal_is(reset, true);
    }
};

SC_MODULE(controller) {
    sc_in_clk clk;
    sc_in<bool> hw_enable;
    sc_out<bool> load_input, load_output;
    sc_out<bool> loop_start, loop_reset;
    sc_out<bool> hw_done;
    sc_in<bool> loop_done;

    void fsm();

    SC_CTOR(controller) {
        SC_CTHREAD(fsm, clk.pos());
    }
};


#endif /* end _MODULES_H_ */