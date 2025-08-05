#ifndef _DH_HW_H_
#define _DH_HW_H_ 1

#include "systemc.h"
#include "digit.h"
#include "modules.h"

SC_MODULE (dh_hw)
{
  sc_in <NN_DIGIT> from_sw0, from_sw1, from_sw2;
  sc_in <NN_HALF_DIGIT> from_sw3;

  sc_out <NN_DIGIT> to_sw0, to_sw1;
  sc_out <NN_HALF_DIGIT> to_sw2;

  sc_in<bool> clk;
  
  sc_in <bool> hw_enable; 
  sc_out <bool> hw_done;

  // register signals
  sc_signal<bool> load_input, load_output;
  sc_signal<NN_DIGIT> R0in_output, R1in_output, R2in_output, R3_extended;
  sc_signal<NN_HALF_DIGIT> R3in_output;
  sc_signal<NN_DIGIT> R0out_input, R1out_input;
  sc_signal<NN_HALF_DIGIT> R2out_input;

  // interconnect signals for multipliers and splitters
  sc_signal<NN_DIGIT> c, c_high, c_low;
  sc_signal<NN_DIGIT> aHigh, u, v;
  // interconnect signals for the multiplexer and comparator
  sc_signal <NN_DIGIT> u_high, t0_sub, u_sub, t1_decremented, t1_after_conditional_decrement;
  sc_signal <NN_DIGIT> max_val, one;
  sc_signal<bool> comparator_sig;
  // interconnect signals for the final subtraction
  sc_signal<NN_DIGIT> u_upper_half, stub, t1_minus_u_out, t1_output;
  // looper signals
  sc_signal<bool> loop_start, loop_done, loop_reset;
  sc_signal<NN_DIGIT> loop_t0, loop_t1;
  sc_signal<NN_HALF_DIGIT> loop_aHigh;


  //modules
  //input registers
  reg R0in, R1in, R2in;
  half_reg R3in;
  //output registers
  reg R0out, R1out;
  half_reg R2out;

  //first split and multiply
  zero_extender R3_extend;
  splitter c_splitter;
  multiplier u_multiplier, v_multiplier;

  //conditional decrement
  subtractor t0_subtractor, u_subtractor, t1_subtractor;
  bit_shift u_shifter;
  comparator dec_comparator;
  multiplexor dec_multiplexor;

  //final subtraction
  subtractor t1_minus_u, t1_minus_v;
  splitter u_splitter;

  //seperating loop module
  loop_engine loop;

  //controller
  controller controller_inst;

  void process_hw();
  
  SC_CTOR (dh_hw):
  c_splitter("c_splitter"),
  u_multiplier("u_multiplier"), v_multiplier("v_multiplier"),
  t0_subtractor("t0_subtractor"), u_subtractor("u_subtractor"),
  t1_subtractor("t1_subtractor"), u_shifter("u_shifter"),
  dec_comparator("dec_comparator"), dec_multiplexor("dec_multiplexor"),
  t1_minus_u("t1_minus_u"), t1_minus_v("t1_minus_v"), u_splitter("u_splitter")
  , R0in("R0in"), R1in("R1in"), R2in("R2in"), R3in("R3in")
  , R0out("R0out"), R1out("R1out"), R2out("R2out"), R3_extend("R3_extend"),
  loop("loop"), controller_inst("controller_inst")
  {
    max_val.write(0xFFFFFFFF);
    one.write(1);
    //first splitter connection
    c_splitter.in_data(R2in_output);
    c_splitter.out_data_h(c_high);
    c_splitter.out_data_l(c_low);

    R3_extend.in_data(R3in_output);
    R3_extend.out_data(R3_extended);

    //first and second multiplier connections
    u_multiplier.in_data_1(R3_extended);
    u_multiplier.in_data_2(c_low);
    u_multiplier.out_data(u);

    v_multiplier.in_data_1(R3_extended);
    v_multiplier.in_data_2(c_high);
    v_multiplier.out_data(v);

    //u splitter
    u_shifter.in_data(u);
    u_shifter.out_data(u_high);

    //t[0] -= TO_HIGH_HALF (u)
    t0_subtractor.in_data_1(R0in_output);
    t0_subtractor.in_data_2(u_high); 
    t0_subtractor.out_data(t0_sub);

    //MAX_NN_DIGIT - TO_HIGH_HALF (u)
    u_subtractor.in_data_1(max_val);
    u_subtractor.in_data_2(u_high); 
    u_subtractor.out_data(u_sub);

    // ((t[0] -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))

    dec_comparator.in_data_1(t0_sub);
    dec_comparator.in_data_2(u_sub);  
    dec_comparator.less_than_signal(comparator_sig); // Use u_high as the signal to indicate if decrement is needed

    //t[1]--
    t1_subtractor.in_data_1(R1in_output);
    t1_subtractor.in_data_2(one); 
    t1_subtractor.out_data(t1_decremented);

    // if ((t[0] -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))
    dec_multiplexor.in_data_1(R1in_output);
    dec_multiplexor.in_data_2(t1_decremented);
    dec_multiplexor.select(comparator_sig);
    dec_multiplexor.out_data(t1_after_conditional_decrement);

    u_splitter.in_data(u);
    u_splitter.out_data_h(u_upper_half);
    u_splitter.out_data_l(stub);

    t1_minus_u.in_data_1(t1_after_conditional_decrement);
    t1_minus_u.in_data_2(u_upper_half);
    t1_minus_u.out_data(t1_minus_u_out);

    t1_minus_v.in_data_1(t1_minus_u_out);
    t1_minus_v.in_data_2(v);
    t1_minus_v.out_data(t1_output);

    // Register connections
    R0in.in_data(from_sw0);
    R0in.load_data(load_input);
    R0in.out_data(R0in_output);
    R0in.clk(clk);

    R1in.in_data(from_sw1);
    R1in.load_data(load_input);
    R1in.out_data(R1in_output);
    R1in.clk(clk);

    R2in.in_data(from_sw2);
    R2in.load_data(load_input);
    R2in.out_data(R2in_output);
    R2in.clk(clk);

    R3in.in_data(from_sw3);
    R3in.load_data(load_input);
    R3in.out_data(R3in_output);
    R3in.clk(clk);

    R0out.in_data(R0out_input);
    R0out.load_data(load_output);
    R0out.out_data(to_sw0);
    R0out.clk(clk);

    R1out.in_data(R1out_input);
    R1out.load_data(load_output);
    R1out.out_data(to_sw1);
    R1out.clk(clk);

    R2out.in_data(R2out_input);
    R2out.load_data(load_output);
    R2out.out_data(to_sw2);
    R2out.clk(clk);

    //looper connection
    loop(clk);
    loop.start(loop_start);
    loop.reset(loop_reset);
    loop.cHigh(c_high);
    loop.cLow(c_low);
    loop.t0_in(t0_sub); 
    loop.t1_in(t1_output);
    loop.aHigh_in(R3in_output);  

    loop.t0_out(R0out_input);
    loop.t1_out(R1out_input);
    loop.aHigh_out(R2out_input);
    loop.done(loop_done);

    controller_inst(clk);
    controller_inst.hw_enable(hw_enable);
    controller_inst.loop_done(loop_done);

    controller_inst.load_input(load_input);
    controller_inst.load_output(load_output);
    controller_inst.loop_start(loop_start);
    controller_inst.loop_reset(loop_reset);
    controller_inst.hw_done(hw_done);

    SC_CTHREAD(process_hw, clk.pos());
  }
  
};

#endif /* end _DH_HW_H_ */
