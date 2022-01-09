#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;


bool IsSubdomain(string_view subdomain, string_view domain) {
    auto i = 0;
    auto j = 0;
    while (i < subdomain.size() && j < domain.size()) {
        if (subdomain[i++] != domain[j++]) {
            return false;
        }
    }
    return (i >= subdomain.size() && domain[j] == '.')
        || (j >= domain.size() && subdomain[i] == '.');
}


vector<string> readDomains() {
    size_t count;
    cin >> count;

    vector<string> domains;
    for (size_t i = 0; i < count; ++i) {
        string domain;
        cin >> domain;
        domains.push_back(domain);
    }
    return domains;
}

int main() {
    vector<string> bannedDomains = readDomains();
    vector<string> domainsToCheck = readDomains();

    for (string& domain : bannedDomains) {
        reverse(begin(domain), end(domain));
    }
    for (string& domain : domainsToCheck) {
        reverse(begin(domain), end(domain));
    }
    sort(begin(bannedDomains), end(bannedDomains));

    size_t insertPos = 0;
    for (string& domain : bannedDomains) {
        if (insertPos == 0 || !IsSubdomain(domain, bannedDomains[insertPos - 1])) {
            swap(bannedDomains[insertPos++], domain);
        }
    }
    bannedDomains.resize(insertPos);
    
    for (const string_view domain : domainsToCheck) {
        if (const auto it = upper_bound(begin(bannedDomains), end(bannedDomains), domain);
            it != begin(bannedDomains) && (domain == *prev(it) || IsSubdomain(domain, *prev(it)))) {
            cout << "Bad" << endl;
        }
        else {
            cout << "Good" << endl;
        }
    }
    return 0;
}