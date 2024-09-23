-- ------------------------------------------------------
-- tb_system.vhd: testbench for system with processor;
--      provides clock and reset inputs, which are
--      sufficient to simulate system operation and
--      allow observation of address/data/control lines
--
-- Naraig Manjikian
-- September 2022
-- ------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

use work.my_components.all;

-- ------------------------------------------------------

entity tb_system is
end entity;

-- ------------------------------------------------------

architecture testbench of tb_system is


signal    clk             : std_logic;
signal    reset_n         : std_logic;
signal    ifetch_out      : std_logic;
signal    mem_addr_out    : std_logic_vector(31 downto 0);
signal    data_from_procr : std_logic_vector(31 downto 0);
signal    data_to_procr   : std_logic_vector(31 downto 0);
signal    mem_read        : std_logic;
signal    mem_write       : std_logic;
signal    rom_active      : std_logic;
signal    ram_active      : std_logic;

-- ------------------------------------------------------

begin

the_system : system
  port map (clk, reset_n, ifetch_out, mem_addr_out,
            data_from_procr, data_to_procr,
            mem_read, mem_write, rom_active, ram_active);

the_reset : process
begin
  reset_n <= '0';
  wait for 10 ns;
  reset_n <= '1';
  wait;
end process;

the_clock : process
begin
  clk <= '1';
  wait for 10 ns;
  clk <= '0';
  wait for 10 ns;
end process;

end architecture;
