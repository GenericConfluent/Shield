#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <string>

namespace mp {
    class AssetManager {
    public:
        template <typename Asset>
        void load(std::string id, std::string file_name) {
            Asset asset;
            if (asset.loadFromFile(file_name)) {
                this->store[{typeid(Asset), id}] = std::make_shared<Asset>(std::move(asset));
            }
        }

        template <typename Asset>
        const Asset& get(std::string id) {
            return *std::static_pointer_cast<Asset>(this->store.at({typeid(Asset), id}));
        }
    private:
        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator () (const std::pair<T1, T2> &p) const {
                auto h1 = std::hash<T1>{}(p.first);
                auto h2 = std::hash<T2>{}(p.second);
                return h1 ^ (h2 << 1);
            }
        };
        std::unordered_map<std::pair<std::type_index, std::string>, std::shared_ptr<void>, pair_hash> store;
    };
}
