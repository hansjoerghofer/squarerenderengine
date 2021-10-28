import math

def binomial(n, k):
    return math.factorial(n) / ( math.factorial(k) * math.factorial( n - k ) )

def interpolate_offset(o1, w1, o2, w2):
    return (o1 * w1 + o2 * w2) / (w1 + w2)

def gaussianWeights(index, omitEdges = 0):
    coeffs = list()
    sum = 0
    for k in range(index+1):

        # remove egdes for LDR weights
        if k < omitEdges or k > (index - omitEdges):
            continue

        w = binomial(index, k)
        coeffs.append(w)
        sum += w

    print(coeffs)

    weights = [ x / sum for x in coeffs]
    return weights

index = 32
omitEdges = 0 # for LDR cases edges don't have much influence
fullWeights = gaussianWeights(index, omitEdges)
numFullWeights = len(fullWeights)

# positive half of weights
weights = fullWeights[(int)(numFullWeights/2):numFullWeights]
offsets = list(range(len(weights)))

print(weights)
print(offsets)

print("## Linear interpolation ##")

iWeights = list()
iOffsets = list()

# center weight
iWeights.append(weights[0])
iOffsets.append(offsets[0])

# pairwise combindation of remaining weights
for k in range((int)((len(weights)-1) / 2)):
    i = (k*2)+1
    iWeights.append(weights[i] + weights[i+1])
    iOffsets.append(interpolate_offset(offsets[i], weights[i], offsets[i+1], weights[i+1]))

print(iWeights)
print(iOffsets)

print("## Gaussian with {0}x{0} kernel -> {1} samples".format(numFullWeights, len(iWeights)))