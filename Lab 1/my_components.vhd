-- ------------------------------------------------------
-- my_components.vhd: package file to provide component
--             declarations for use in design hierarchy
--
-- Naraig Manjikian
-- February 2012
-- ------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

-- ------------------------------------------------------------------

package my_components is

component system 
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
end component;

component reg32
  port (
    clk     : in std_logic;
    reset_n : in std_logic;
    en      : in std_logic;
    d       : in std_logic_vector(31 downto 0);
    q       : out std_logic_vector
  );
end component;

component processor
  port (
    clk          : in std_logic;
    reset_n      : in std_logic;
    mem_addr_out : out std_logic_vector(31 downto 0);
    mem_data_out : out std_logic_vector(31 downto 0);
    mem_data_in  : in  std_logic_vector(31 downto 0);
    mem_read     : out std_logic;
    mem_write    : out std_logic;
    ifetch_out   : out std_logic
  );
end component;

component decoder_2to4
  port (
    x_in      : in std_logic_vector(1 downto 0);
    y_out     : out std_logic_vector(3 downto 0)
  );
end component;

component system_address_decoder
  port (
    addr_in    : in std_logic_vector(15 downto 4);
    rom_select : out std_logic;
    ram_select : out std_logic
  );
end component;

component rom256
  port (
    cs         : in std_logic;
    addr_in    : in std_logic_vector(7 downto 0);
    data_out   : out std_logic_vector(31 downto 0)
  );
end component;

component ram4
  port (
    clk        : in std_logic;
    reset_n    : in std_logic;
    cs         : in std_logic;
    we         : in std_logic;
    addr_in    : in std_logic_vector(1 downto 0);
    data_in    : in std_logic_vector(31 downto 0);
    data_out   : out std_logic_vector(31 downto 0)
  );
end component;

end package;
