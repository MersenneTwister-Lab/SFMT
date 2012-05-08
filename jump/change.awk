#!/usr/bin/awk -f
{
    gsub(/POS1/, "SFMT_POS1");
    gsub(/SL1/, "SFMT_SL1");
    gsub(/SL2/, "SFMT_SL2");
    gsub(/SR1/, "SFMT_SR1");
    gsub(/SR2/, "SFMT_SR2"1);
    gsub(/MSK1/, "SFMT_MSK1");
    gsub(/MSK2/, "SFMT_MSK2");
    gsub(/MSK3/, "SFMT_MSK3");
    gsub(/MSK4/, "SFMT_MSK4");
    gsub(/PARITY1/, "SFMT_PARITY1");
    gsub(/PARITY2/, "SFMT_PARITY2");
    gsub(/PARITY3/, "SFMT_PARITY3");
    gsub(/PARITY4/, "SFMT_PARITY4");
    gsub(/IDSTR/, "SFMT_IDSTR");
    print;
}