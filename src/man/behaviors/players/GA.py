#Jack Truskowski
#Robotics Final Project - Fall 2016
#ML for walk engine

import random

from deap import base
from deap import creator
from deap import tools

#evaluates the fitness of an individual
def evaluate(individual):

    #WALKING SHOULD BE DONE HERE

    a = 1/individual[0]
    b = abs(2.5-individual[1])
    return a,b

'''
#crosses over 2 individuals and returns their 2 children
def crossover(ind1, ind2):
    child1, child2 = [toolbox.clone(ind) for ind in(ind1, ind2)]
    tools.cxblend(child1, child2, 0.5)
    del child1.fitness.values
    del child2.fitness.values
    return child1, child2
'''


#Fitness weights: 1.0 = trying to maximize this component
# -1.0 = trying to minimize this component
creator.create("FitnessFunc", base.Fitness, weights=(1.0,-1.0))
creator.create("Individual", list, fitness=creator.FitnessFunc)

#size of individual attributes (should be one for each walk param)
IND_SIZE = 10

#initialize an 'individual' type
toolbox = base.Toolbox()
toolbox.register("attr_float", random.random)
toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_float, n=IND_SIZE)
toolbox.register("mate", tools.cxTwoPoint)
toolbox.register("mutate", tools.mutGaussian, mu=0, sigma=1, indpb=0.2)
toolbox.register("select", tools.selTournament, tournsize=3)
toolbox.register("evaluate", evaluateInd)

population = []

#create 10 individuals
for i in range(0,1):
    anInd = toolbox.individual();
    anInd.fitness.values = evaluate(anInd)
    print anInd.fitness.valid
    print anInd.fitness
    population.append(anInd)
