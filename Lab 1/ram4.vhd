-- ------------------------------------------------------
-- ram4.vhd: implements a 4-word writable memory;
--           a *word* address is accepted as input, hence
--           the byte-offset bits from the full processor
--           address should be left out of the input to
--           this entity
--
-- Naraig Manjikian
-- August 2012
-- ------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

use work.my_components.all;  -- for 2-to-4 decoder used below

-- ------------------------------------------------------------------

entity ram4 is
  port (
    clk        : in std_logic;
    reset_n    : in std_logic;
    cs         : in std_logic;
    we         : in std_logic;
    addr_in    : in std_logic_vector(1 downto 0);
    data_in    : in std_logic_vector(31 downto 0);
    data_out   : out std_logic_vector(31 downto 0)
  );
end entity;

-- ------------------------------------------------------------------

architecture behavior of ram4 is

-- signals for outputs for each location
signal data0, data1, data2, data3 : std_logic_vector(31 downto 0);

-- signal for output of storage
signal ram_out : std_logic_vector(31 downto 0);

-- signals for selecting and write-enabling each location
signal sel, en : std_logic_vector(3 downto 0);


begin  -- start of architecture body

-- instantiate decoder for selection signals that depend on address input

the_decoder : decoder_2to4
  port map (
    x_in  => addr_in,
    y_out => sel
  );

-- define individual write-enable signals based on decoder outputs

en(0) <= sel(0) and we;
en(1) <= sel(1) and we;
en(2) <= sel(2) and we;
en(3) <= sel(3) and we;

-- instantiate registers to hold data for the locations in this memory
--   with each register having its dedicated write-enable input,
--   and separate data output for possible selection external entity output,
--   but all using the same external data input

data0_reg : reg32 port map (clk, reset_n, en(0), data_in, data0);
data1_reg : reg32 port map (clk, reset_n, en(1), data_in, data1);
data2_reg : reg32 port map (clk, reset_n, en(2), data_in, data2);
data3_reg : reg32 port map (clk, reset_n, en(3), data_in, data3);

-- define internal signal for RAM output

ram_out <=   data0 when (sel(0) = '1')
        else data1 when (sel(1) = '1')
        else data2 when (sel(2) = '1')
        else data3; --  (sel(3) = '1')

-- define external output from entity when active and not performing a write,
--   otherwise provide X"00000000" as output

data_out <=   ram_out when (cs = '1' and we = '0')
         else (others => '0');
		
end architecture;
