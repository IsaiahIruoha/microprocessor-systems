-- ------------------------------------------------------
-- decoder_2to4.vhd: implements a 2-to-4 decoder
--
-- Naraig Manjikian
-- August 2012
-- ------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

-- ------------------------------------------------------------------

entity decoder_2to4 is
  port (
    x_in      : in std_logic_vector(1 downto 0);
    y_out     : out std_logic_vector(3 downto 0)
  );
end entity;

-- ------------------------------------------------------------------

architecture rtl of decoder_2to4 is

begin

-- the following signal assignment statement is incomplete;
--  include the remaining parts so that it properly decodes
--  the 00, 01, 10, and 11 inputs to the corresponding one-hot outputs

y_out <=   "0001" when (x_in = "00")
      else "0010" when (x_in = "01") 
      else "0100" when (x_in = "10")
      else "1000"; --  (x_in = "11")

end architecture;
