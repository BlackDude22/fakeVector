# Fake vector

## Įdomūs dalykai

```cpp 
template <class InputIterator, typename = std::_RequireInputIter<InputIterator>>
```

Nuo c++11:
```cpp
void push_back(value_type&& __x){
emplace_back(std::move(__x)); 
}
```


## Spartos analizė

Atlikta po 100 bandymų su 10^7 std::string l-value tipo push_back'ų.

| Optimization flag | Fake time | Real time | Difference |
|-------------------|-----------|-----------|------------|
| None | 1.0945s | 1.0389s | -5.35% |
| O1 | 0.8379s | 0.8247s | -1.59% |
| O2 | 0.6353s | 0.6157s | -3.17% |
| O3 | 0.6412s | 0.6242s | -2.71% |
