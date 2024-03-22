#in (
    10 15
)

#out (
    00
)

main:
    smld IR0 #in   1
    SMLD IR1 #in+1 1
    ADD  IR2 IR0   IR1
    SMST IR2 #out  1
    STOP