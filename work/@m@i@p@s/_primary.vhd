library verilog;
use verilog.vl_types.all;
entity MIPS is
    port(
        R2_output       : out    vl_logic_vector(31 downto 0);
        MemRead         : out    vl_logic;
        MemWrite        : out    vl_logic;
        iBlkRead        : out    vl_logic;
        iBlkWrite       : out    vl_logic;
        dBlkRead        : out    vl_logic;
        dBlkWrite       : out    vl_logic;
        data_write_2DM  : out    vl_logic_vector(31 downto 0);
        block_write_2DM : out    vl_logic_vector(255 downto 0);
        block_write_2IM : out    vl_logic_vector(255 downto 0);
        block_read_fDM  : in     vl_logic_vector(255 downto 0);
        block_read_fIM  : in     vl_logic_vector(255 downto 0);
        Instr1_fIM      : in     vl_logic_vector(31 downto 0);
        Instr2_fIM      : in     vl_logic_vector(31 downto 0);
        data_address_2DM: out    vl_logic_vector(31 downto 0);
        CLK             : in     vl_logic;
        RESET           : in     vl_logic;
        R2_input        : in     vl_logic_vector(31 downto 0);
        PC_init         : in     vl_logic_vector(31 downto 0)
    );
end MIPS;
