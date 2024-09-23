-- ------------------------------------------------------
-- system.vhd: top-level entity incorporating an instance
--     of a processor and other elements;
--     output signals allow for observation of operation
--
-- Naraig Manjikian
-- February 2012; revised September 2022
-- ------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

-- provides declarations for components used inside this entity
use work.my_components.all;


-- ------------------------------------------------------

entity system is
  port (
    clk             : in std_logic;
    reset_n         : in std_logic;
    ifetch_out      : out std_logic;
    mem_addr_out    : out std_logic_vector(31 downto 0);
    data_from_procr : out std_logic_vector(31 downto 0);
    data_to_procr   : out std_logic_vector(31 downto 0);
    mem_read        : out std_logic;
    mem_write       : out std_logic;
    rom_active      : out std_logic;
    ram_active      : out std_logic
  );
end entity;

-- ------------------------------------------------------

architecture synth of system is

-- address from processor
signal internal_addr : std_logic_vector(31 downto 0);

-- signal asserted from processor in cycle where a write is to be performed
signal internal_write : std_logic;

-- data from processor that is destined to a memory device
signal data_to_mem: std_logic_vector(31 downto 0);

-- data from memory that is destined to the processor
signal data_from_mem : std_logic_vector(31 downto 0);

-- vector output from the decoder for the address space
signal dec_out : std_logic_vector(3 downto 0);

-- individual chip-select signals for memory devices for convenience
signal ram_select, rom_select : std_logic;

-- data outputs from individual memory devices
signal ram_data_out, rom_data_out : std_logic_vector(31 downto 0);


begin  -- start of architecture body

-- instantiate the processor; map its ports to toplevel and internal signals

the_processor : processor
  port map (
    clk          => clk,
    reset_n      => reset_n,
    mem_addr_out => internal_addr,
    mem_data_out => data_to_mem,
    mem_data_in  => data_from_mem,
    mem_read     => mem_read,
    mem_write    => internal_write,
    ifetch_out   => ifetch_out
  );

-- instantiate the system-specific address decoding support

the_system_address_decoder : system_address_decoder
  port map (
      addr_in => internal_addr(15 downto 4),
      rom_select => rom_select,
      ram_select => ram_select
  );

 
-- instantiate the ROM;
--   to access one of the 256 words, 8 address bits are needed,
--   and because a *word* address is fed to the ROM, ignore address bits 1..0,
--   hence we use address bits 9..2 as the inputs to the ROM

the_rom : rom256
  port map (
    cs => rom_select,
    addr_in => internal_addr(9 downto 2),
    data_out => rom_data_out
  );

-- instantiate the RAM;
--   to access one of 4 words, 2 address bits are needed,
--   and because a *word* address is fed to the RAM, ignore address bits 1..0,
--   hence we use address bits 3..2 as the inputs to the RAM

the_ram : ram4
  port map (
    clk => clk,
    reset_n => reset_n,
    cs => ram_select,
    we => internal_write,
    addr_in => internal_addr(3 downto 2),
    data_in => data_to_mem,
    data_out => ram_data_out   
  );

-- because all of the addressable devices above have been designed
--  to always provide X"00000000" as output when not selected, it is possible
--  to simply OR together all of the device outputs; at most one of them
--  will have any logic-1 bits that, when ORed with all-zero outputs from
--  all of the non-selected devices, will appear in the final output value

data_from_mem <= rom_data_out or ram_data_out;  -- vectors are ORed bit by bit

-- finally, make signal assignments for the remaining toplevel ports
--  to allow the internal behavior to be observed

data_from_procr <= data_to_mem;
mem_addr_out    <= internal_addr;
mem_write       <= internal_write;
data_to_procr   <= data_from_mem;
rom_active      <= rom_select;
ram_active      <= ram_select;

end architecture;
