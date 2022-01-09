void PrintStats(vector<Person> persons) {
    cout << "Median age = " << ComputeMedianAge(persons.begin(), persons.end()) << std::endl;
    auto beginMale = partition(persons.begin(), persons.end(), [](const Person& person) {return person.gender == Gender::FEMALE; });
    cout << "Median age for females = " << ComputeMedianAge(persons.begin(), beginMale) << std::endl;
    cout << "Median age for males = " << ComputeMedianAge(beginMale, persons.end()) << std::endl;
    auto beginUnemployedFemale = partition(persons.begin(), beginMale, [](const Person& person) {return person.is_employed; });
    cout << "Median age for employed females = " << ComputeMedianAge(persons.begin(), beginUnemployedFemale) << std::endl;
    cout << "Median age for unemployed females = " << ComputeMedianAge(beginUnemployedFemale, beginMale) << std::endl;
    auto beginUnemployedMale = partition(beginMale, persons.end(), [](const Person& person) {return person.is_employed; });
    cout << "Median age for employed males = " << ComputeMedianAge(beginMale, beginUnemployedMale) << std::endl;
    cout << "Median age for unemployed males = " << ComputeMedianAge(beginUnemployedMale, persons.end()) << std::endl;
}