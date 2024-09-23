-- ------------------------------------------------------
-- rom256.vhd: implements a 256-word read-only memory
--             that functions combinationally to support
--             single-cycle accesses; accepts a *word*
--             address, hence the byte-offset bits from
--             full processor address should be left out
--             of address input to this entity
-- 
-- Naraig Manjikian
-- August 2012
-- ------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

-- ------------------------------------------------------------------

entity rom256 is
  port (
    cs         : in std_logic;
    addr_in    : in std_logic_vector(7 downto 0);
    data_out   : out std_logic_vector(31 downto 0)
  );
end entity;

-- ------------------------------------------------------------------

architecture behavior of rom256 is

-- define a new type for an array of 256 words
type word_array_type is array (0 to 255) of std_logic_vector(31 downto 0);

-- define an instance of a 256-word array with specified values
signal storage : word_array_type :=
(
   X"0042af04",      --  movi    at,2748
   X"004a8015",      --  stw     at,10752(zero)
   X"004a8017",      -- ldw     at,10752(zero)
   X"003ffc06",      --  br      0 <_gp+0xffff7000>,              -- sample hexadecimal word at location 0
  others => (others => '0') -- fill remainder with X"00000000"
);

-- define an internal signal that is the integer value of
--  the bit vector provided as the address input
signal address_index : integer;


begin         -- start of architecture body

-- convert address bit vector to integer value

address_index <= to_integer (unsigned (addr_in));

-- use the integer index to access the desired word in the array,
--  or produce X"00000000" as output when not selected

data_out <=   storage(address_index) when (cs = '1')
         else (others => '0');

end architecture;
