def powerset(seq):
    if len(seq) <= 1:
        yield seq
        yield []
    else:
        for item in powerset(seq[1:]):
            yield [seq[0]]+item
            yield item
def create_d_ball_filtration(d):
    points = list(range(0, d + 1))
    subsets = sorted(list(powerset(points)), key=lambda x: len(x))[1:]
    filtration = [[x for x in subsets if len(x) == i] for i in range(1, d+2)]
    return filtration
def print_filtration(f):
    for t, step in enumerate(f):
        for s in step:
            print("{} {} {}".format(t*1.0, len(s)-1, " ".join([str(x) for x in s])))
def print_filtration_to_file(f, filename):
    with open(filename, 'w') as file:
        for t, step in enumerate(f):
            for s in step:
                file.write("{} {} {}\n".format(t*1.0, len(s)-1, " ".join([str(x) for x in s])))

for d in range(0, 11):
    d_ball = create_d_ball_filtration(d)

    #print('{}-ball:'.format(d))
    print_filtration_to_file(d_ball, '{}-ball-filtration.txt'.format(d))

    d_sphere = create_d_ball_filtration(d+1)[:-1]
    #print('\n{}-sphere:'.format(d))
    print_filtration_to_file(d_sphere, '{}-sphere-filtration.txt'.format(d))
