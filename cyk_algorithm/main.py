class Rule:
    def __init__(self, variable, product):
        self.variable = variable
        self.product = product
        # If the length of the product is 1, it will be a terminal,
        # otherwise it must be a pair of variables
        self.terminal = len(product) == 1

def cyk(test_string, rules):
    test_string_length = len(test_string)
    # TODO: if string length is 1, check if the starting rule variable contains that terminal

    variable_rules = []
    terminal_rules = []
    for rule in rules:
        if rule.terminal:
            terminal_rules.append(rule)
        else:
            variable_rules.append(rule)

    # table = defaultdict(set)
    table = []
    for i in range(test_string_length):
        table.append([])
        for _ in range(test_string_length):
            table[i].append(set())

    # Initial row, checking for which variables produce the terminal in the string
    for i in range(test_string_length):
        for t in terminal_rules:
            if test_string[i] == t.product:
                table[0][i].add(t.variable)

    for i in range(1, test_string_length):
        for j in range(test_string_length - i):
            for k in range(i):
                left = table[k][j]
                right = table[i-k-1][j+k+1]

                if len(left) != 0 and len(right) != 0:
                    table_entry = set()
                    for l in left:
                        for r in right:
                            table_entry.add(l + r)
                    for x in table_entry:
                        for v in variable_rules:
                            if v.product == x:
                                table[i][j].add(v.variable)

    return variable_rules[0].variable in table[test_string_length-1][0]

if __name__ == "__main__":
    rule_count = input()
    n = int(rule_count)
    rule_strings = []
    for i in range(n):
        rule_strings.append(input())

    rules = []
    for r in rule_strings:
        parts = r.split()
        rules.append(Rule(parts[0], parts[1]))

    test_string_count = int(input())
    test_strings = []
    for t in range(test_string_count):
        test_strings.append(input())

    for test_string in test_strings:
        can_be_generated = cyk(test_string, rules)
        if can_be_generated:
            print("yes")
        else:
            print("no")
