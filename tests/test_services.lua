local llnet = require"llnet"

print("llnet.Services.http = ", llnet.Services.http)
print("llnet.Services.byname('http') = ", llnet.Services.byname('http'))
print("llnet.Services[80] = ", llnet.Services[80])
print("llnet.Services.byport(80) = ", llnet.Services.byport(80))

