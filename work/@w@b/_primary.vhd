library verilog;
use verilog.vl_types.all;
entity WB is
    port(
        CLK             : in     vl_logic;
        RESET           : in     vl_logic;
        do_writeback1_PR: out    vl_logic;
        writeRegister1_PR: out    vl_logic_vector(4 downto 0);
        writeData1_PR   : out    vl_logic_vector(31 downto 0);
        do_writeback1   : in     vl_logic;
        aluResult1_OUT  : out    vl_logic_vector(31 downto 0);
        writeRegister1  : in     vl_logic_vector(4 downto 0);
        writeRegister1_OUT: out    vl_logic_vector(4 downto 0);
        writeData1_OUT  : out    vl_logic_vector(31 downto 0);
        do_writeback1_OUT: out    vl_logic;
        aluResult1      : in     vl_logic_vector(31 downto 0);
        Data_input1     : in     vl_logic_vector(31 downto 0);
        MemtoReg1       : in     vl_logic
    );
end WB;
