# Chapter 25 Question 3 -- Summary of Changes

### vectors.lox
```lox
fun makeVector(x, y) {
  return x * 1000000 + y;
}

fun vecX(vec) {
  var x = 0;
  while (vec >= 1000000) {
    vec = vec - 1000000;
    x = x + 1;
  }
  return x;
}

fun vecY(vec) {
  while (vec >= 1000000) {
    vec = vec - 1000000;
  }
  return vec;
}

fun addVectors(a, b) {
  return makeVector(vecX(a) + vecX(b), vecY(a) + vecY(b));
}

var a = makeVector(1, 2);
var b = makeVector(3, 4);

print vecX(a); // 1
print vecY(a); // 2
print vecX(b); // 3
print vecY(b); // 4

var c = addVectors(a, b);
print vecX(c); // 4
print vecY(c); // 6
```
