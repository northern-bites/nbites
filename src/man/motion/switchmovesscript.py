# Script for switch sweet moves from right -> left and vice versa

import sys
import getopt
import fileinput
import re



def switch_move(file_name, move_name):
    openfile = open(file_name)
    file_lines = openfile.readlines()
    openfile.close()

    name_index = 0
    for index, line in enumerate(file_lines):
        if line.find(move_name) >= 0:
            name_index = index

    print "Name index is ", name_index
    line_index = name_index
    line_index +=1

    NEW_MOVE = re.compile('[A-Za-z]')

    write_file = open(file_name, 'a')
    new_move = switch_move_name(move_name)

    write_file.write("\n"+ new_move+" = (\n")
    chains = []

    while not NEW_MOVE.match(file_lines[line_index] ):
        while empty_or_comment(file_lines[line_index]):
            line_index +=1
        if NEW_MOVE.match(file_lines[line_index] ): break

        for i in xrange(4):
            while empty_or_comment(file_lines[line_index]):
                line_index +=1
            if NEW_MOVE.match(file_lines[line_index] ): break

            chains.append(file_lines[line_index])
            line_index +=1
        if NEW_MOVE.match(file_lines[line_index] ): break

        split_chains(chains[0],chains[1],chains[2],chains[3])
        chains = []
        write_move(write_file, move_name)


def empty_or_comment(string):
    COMMENT = re.compile('\s*#')
    return remove_chars(string, " \n") == "" or \
        COMMENT.match(string)


def write_move(write_file, move_name):
    new_larm, new_lleg, new_rleg, new_rarm = build_new_chains()

    write_file.write(new_larm)
    write_file.write(new_lleg)
    write_file.write(new_rleg)
    write_file.write(new_rarm)

def build_new_chains():
    c = " , "
    endl = "),\n"
    new_larm = "((" + str(rsp) +c+ str(-rsr) +c+ str(-rey) +c+ str(-rer) + endl

    new_lleg = "(" + str(rhyp) +c+  str(-rhr) +c+ str(rhp) +c+ str(rkp)+c+ \
        str(rap) +c+ str(-rar) + endl

    new_rleg = "(" + str(lhyp) +c+  str(-lhr) +c+ str(lhp) +c+ str(lkp)+c+ \
        str(lap) +c+ str(-lar) + endl

    new_rarm = "("+ str(lsp) +c+ str(-lsr) +c+ str(-ley) +c+ str(-ler) +"), " \
        + str(time) +c+ str(int(interpolation)) + endl + '\n'

    return new_larm, new_lleg, new_rleg, new_rarm

def switch_move_name(move_name):
    if move_name.find('LEFT') >= 0:
        return move_name.replace('LEFT','RIGHT')
    else :
        return move_name.replace('RIGHT','LEFT')


def split_chains(larm, lleg, rleg, rarm):

    split_larm = larm.split(',')
    split_lleg = lleg.split(',')
    split_rleg = rleg.split(',')
    split_rarm = rarm.split(',')




    split_larm = split_chain(split_larm)
    split_lleg = split_chain(split_lleg)
    split_rleg = split_chain(split_rleg)
    split_rarm = split_chain(split_rarm)

    assign_joints(split_larm, split_lleg, split_rleg, split_rarm)

def split_chain(chain):
    parens = '()\n'
    for i in xrange(len(chain)):
        chain[i] = remove_chars(chain[i],parens)
        if chain[i] == '':
            chain.remove('')
            continue
        chain[i] = float(chain[i])
    return chain

def assign_joints(split_larm, split_lleg, split_rleg, split_rarm):
    global lsp, lsr, ley, ler
    global lhyp, lhr, lhp, lkp, lap, lar
    global rhyp, rhr, rhp, rkp, rap, rar
    global rsp, rsr, rey, rer
    global time, interpolation

    lsp, lsr, ley, ler = split_larm
    lhyp, lhr, lhp, lkp, lap, lar = split_lleg
    rhyp, rhr, rhp, rkp, rap, rar = split_rleg
    rsp, rsr, rey, rer, time, interpolation = split_rarm


def main(argv):
    move_name = " "
    file_name = "SweetMoves.py"


    try:
        opts, args = getopt.getopt(argv, "hf:m:")
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-h":
            usage()
            sys.exit()

        elif  opt == "-f":
            file_name = arg

        elif opt == "-m":
            move_name = arg

    if move_name == "":
        usage()
        sys.exit()

    print "move is ", move_name
    print "file is ", file_name

    switch_move(file_name, move_name)

def remove_chars(string, chars):
    _allchars = "".join([chr(x) for x in range(256)])
    return string.translate(_allchars, chars)



if __name__ == "__main__":
    main(sys.argv[1:])
