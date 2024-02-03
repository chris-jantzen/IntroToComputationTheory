class Rule:
    def __init__(self, variable, product):
        self.variable = variable
        self.product = product
        # If the length of the product is 1, it will be a terminal,
        # otherwise it must be a pair of variables
        self.terminal = len(product) == 1

def cyk(test_string, start_rule_variable, variable_rules, terminal_rules):
    test_string_length = len(test_string)

    # Can never be in the grammar because the input requirements specify that terminals are only
    # allowed to be a-z for this problem
    if test_string_length < 1:
        return False

    # Quickly handle cases where the test_string is only 1 character long
    if test_string_length == 1:
        for r in terminal_rules:
            if r.variable == start_rule_variable and r.product == test_string[0]:
                return True
        return False

    table = []
    for i in range(test_string_length):
        table.append([])
        for _ in range(test_string_length, i, -1):
            table[i].append(set())

    # Initial row, checking for which variables produce the terminal in the string
    for i in range(test_string_length):
        for t in terminal_rules:
            if test_string[i] == t.product:
                table[0][i].add(t.variable)

    for i in range(1, test_string_length):
        for j in range(test_string_length - i):
            for k in range(i):
                left_substring_portion_vars = table[k][j]
                right_substring_portion_vars = table[i-k-1][j+k+1]

                if len(left_substring_portion_vars) != 0 and len(right_substring_portion_vars) != 0:
                    var_pair = set()
                    for l in left_substring_portion_vars:
                        for r in right_substring_portion_vars:
                            var_pair.add(l + r)
                    for x in var_pair:
                        for v in variable_rules:
                            if v.product == x:
                                table[i][j].add(v.variable)

    return start_rule_variable in table[test_string_length-1][0]

def read_input():
    rule_count = input()
    n = int(rule_count)
    rule_strings = []
    for _ in range(n):
        rule_strings.append(input())

    rules = []
    for r in rule_strings:
        parts = r.split()
        rules.append(Rule(parts[0], parts[1]))

    test_string_count = int(input())
    test_strings = []
    for _ in range(test_string_count):
        test_strings.append(input())
    return (rules, test_strings)

if __name__ == "__main__":
    (rules, test_strings) = read_input()

    # Split rules into list of type V -> V1V2 and V -> t for easier comparison in CYK algorithm
    start_rule_variable = rules[0].variable
    variable_rules = []
    terminal_rules = []
    for rule in rules:
        if rule.terminal:
            terminal_rules.append(rule)
        else:
            variable_rules.append(rule)

    for test_string in test_strings:
        can_be_generated = cyk(test_string, start_rule_variable, variable_rules, terminal_rules)
        if can_be_generated:
            print("yes")
        else:
            print("no")
