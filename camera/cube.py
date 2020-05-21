s =[
    (2.0,2.0),
    (-2.0,2.0),
    (-2.0,-2.0),
    (2.0,-2.0)
]

t = [
    (2.0,2.0),
    (0.0,2.0),
    (0.0,0.0),
    (2.0,0.0)
]
l=[]
for v,te in zip(s,t):
    l.append(v+(2.0,) )
    l.append(v+(-2.0,))
print('\n'.join(map(lambda w:"glm::vec3("+",".join(map(lambda w2: str(w2)+'f',w))+"),",l ) ))
