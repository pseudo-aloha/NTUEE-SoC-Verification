pattern_count = 0
line = 0
bugs = {"bug_p" : 0, "bug_q" : 0, "bug_r" : 0, "bug_s" : 0, "bug_t" : 0}
with open("sim.output", "r") as f:
  a = f.readline()
  line += 1
  while len(a) != 0:
    if a[16] == "1":
      bugs["bug_t"] += 1
    if a[17] == "1":
      bugs["bug_s"] += 1
    if a[18] == "1":
      bugs["bug_r"] += 1
    if a[19] == "1":
      bugs["bug_q"] += 1
    if a[20] == "1":
      bugs["bug_p"] += 1
      
    a = f.readline()
    line += 1
      
print("Bug report : ")
print("Bug t Num =", bugs["bug_t"], "\t\tcheck if the input item is equal to the output item when the type out item is not ITEM_NONE")
print("Bug s Num =", bugs["bug_s"], "\t\tcheck if the machine makes the correct exchange for ITEM_C")
print("Bug r Num =", bugs["bug_r"], "\t\tcheck if the machine makes the correct exchange for ITEM_B")
print("Bug q Num =", bugs["bug_q"], "\t\tcheck if the machine makes the correct exchange for ITEM_A")
print("Bug p Num =", bugs["bug_p"], "\twhen the machine cannot make the change, it will output exchange different from the input value")
