/*
 * Copyright 2023 AzgathCore
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "BattlePayData.h"
#include "BattlePayMgr.h"
#include "Containers.h"
#include "DatabaseEnv.h"
#include "ObjectMgr.h"

BattlePayDataStoreMgr::BattlePayDataStoreMgr() = default;

BattlePayDataStoreMgr::~BattlePayDataStoreMgr() = default;

BattlePayDataStoreMgr* BattlePayDataStoreMgr::instance()
{
    static BattlePayDataStoreMgr instance;
    return &instance;
}

namespace
{
    Battlepay::Product emptyList;
    std::vector<Battlepay::ProductGroup> _productGroups;
    std::vector<Battlepay::ShopEntry> _shopEntries;
    std::map<uint32, Battlepay::Product> _products;
    std::map<uint32, Battlepay::DisplayInfo> _displayInfos;
    std::map<uint32, Battlepay::DisplayVisualData> _visualsDatas;
    std::map<uint32, Battlepay::ProductGroupLocale> _productGroupLocales;
    std::map<uint32, BattlePayDisplayInfoLocale> _displayInfoLocales;
}

void BattlePayDataStoreMgr::Initialize()
{
    LoadDisplayInfos();
    LoadDisplayInfoVisuals();
    LoadProduct();
    LoadProductGroups();
    LoadShopEntries();
    LoadProductGroupLocales();
    LoadDisplayInfoLocales();
}

void BattlePayDataStoreMgr::LoadDisplayInfos()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay display info ...");
    _displayInfos.clear();

    auto result = WorldDatabase.PQuery("SELECT DisplayInfoId, CreatureDisplayInfoID, FileDataID, DisplayCardWidth, Flags, Name1, Name2, Name3, Name4, Name5 FROM battlepay_display_info");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        Battlepay::DisplayInfo displayInfo;
        displayInfo.CreatureDisplayInfoID   = fields[1].GetUInt32();
        displayInfo.FileDataID              = fields[2].GetInt32();
        displayInfo.DisplayCardWidth        = fields[3].GetInt32();
        displayInfo.Flags                   = fields[4].GetUInt32();
        displayInfo.Name1                   = fields[5].GetString();
        displayInfo.Name2                   = fields[6].GetString();
        displayInfo.Name3                   = fields[7].GetString();
        displayInfo.Name4                   = fields[8].GetString();
        displayInfo.Name5                   = fields[9].GetString();
        _displayInfos.insert(std::make_pair(fields[0].GetUInt32(), displayInfo));
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battlepay display info in %u ms.", uint64(_displayInfos.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadDisplayInfoVisuals()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay display info visuals ...");
    _visualsDatas.clear();

    auto result = WorldDatabase.PQuery("SELECT DisplayInfoId, DisplayId, VisualId, ProductName FROM battlepay_display_info_visuals");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        Battlepay::DisplayVisualData visualInfo;
        visualInfo.DisplayID       = fields[1].GetUInt32();
        visualInfo.VisualID        = fields[2].GetInt32();
        visualInfo.ProductName     = fields[3].GetString();
        _visualsDatas.insert(std::make_pair(fields[0].GetUInt32(), visualInfo));
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battlepay display info visuals in %u ms.", uint64(_visualsDatas.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadProductGroups()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay product groups ...");
    _productGroups.clear();

    auto result = WorldDatabase.PQuery("SELECT GroupID, Name, IconFileDataID, DisplayType, Ordering FROM battlepay_product_group");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        Battlepay::ProductGroup productGroup;
        productGroup.GroupID        = fields[0].GetUInt32();
        productGroup.Name           = fields[1].GetString();
        productGroup.IconFileDataID = fields[2].GetInt32();
        productGroup.DisplayType    = fields[3].GetUInt8();
        productGroup.Ordering       = fields[4].GetInt32();
        _productGroups.push_back(productGroup);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battlepay product groups in %u ms", uint64(_productGroups.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadProduct()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay products ...");
    _products.clear();

    auto result = WorldDatabase.PQuery("SELECT ProductID, NormalPriceFixedPoint, CurrentPriceFixedPoint, Type, WebsiteType, CustomValue, ChoiceType, Flags, DisplayInfoID, SpellID, CreatureID, ClassMask, ScriptName FROM battlepay_product");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        Battlepay::Product product;
        product.ProductID               = fields[0].GetUInt32();
        product.WebsiteType             = fields[4].GetUInt8();
        if (product.WebsiteType >= Battlepay::MaxWebsiteType)
        {
            TC_LOG_ERROR("SQL", "BattlePayDataStoreMgr: battlepay_product websiteType >= max types - skip loading: type %u; productId: %u ", product.WebsiteType, product.ProductID);
            continue;
        }

        product.NormalPriceFixedPoint   = fields[1].GetUInt64();
        product.CurrentPriceFixedPoint  = fields[2].GetUInt64();
        product.Type                    = fields[3].GetUInt8();
        product.CustomValue             = fields[5].GetUInt64();
        product.ChoiceType              = fields[6].GetUInt8();
        product.Flags                   = fields[7].GetUInt32();
        product.DisplayInfoID           = fields[8].GetUInt32();
        product.SpellID                 = fields[9].GetUInt32();
        product.CreatureID              = fields[10].GetUInt32();
        product.ClassMask               = fields[11].GetUInt32();
        product.ScriptName              = fields[12].GetString();

        _products.insert(std::make_pair(product.ProductID, product));
    } while (result->NextRow());

    result = WorldDatabase.PQuery("SELECT ID, ProductID, ItemID, Quantity, DisplayID, PetResult FROM battlepay_product_item");
    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        auto productID          = fields[1].GetUInt32();
        if (_products.find(productID) == _products.end())
            continue;

        Battlepay::ProductItem productItem;
        productItem.DisplayInfoID = fields[4].GetUInt32();
        if (productItem.DisplayInfoID != 0 && _displayInfos.find(productItem.DisplayInfoID) == _displayInfos.end())
            continue;

        productItem.ItemID      = fields[2].GetUInt32();
        if (!sItemStore[productItem.ItemID])
            continue;

        productItem.ID          = fields[0].GetUInt32();
        productItem.Quantity    = fields[3].GetUInt32();
        productItem.PetResult   = fields[5].GetUInt8();
        _products[productID].Items.push_back(productItem);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battlepay products in %u ms", uint64(_products.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadShopEntries()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay shop entries ...");
    _shopEntries.clear();

    auto result = WorldDatabase.PQuery("SELECT EntryID, GroupID, ProductID, Ordering, VasServiceType, StoreDeliveryType, DisplayInfoID FROM battlepay_shop_entry");
    if (!result)
        return;

    auto oldMsTime = getMSTime();

    do
    {
        auto fields = result->Fetch();

        Battlepay::ShopEntry shopEntry;
        shopEntry.EntryID           = fields[0].GetUInt32();
        shopEntry.GroupID           = fields[1].GetUInt32();
        shopEntry.ProductID         = fields[2].GetUInt32();
        shopEntry.Ordering          = fields[3].GetInt32();
        shopEntry.VasServiceType    = fields[4].GetUInt32();
        shopEntry.StoreDeliveryType = fields[5].GetUInt8();
        shopEntry.DisplayInfoID     = fields[6].GetUInt32();
        _shopEntries.push_back(shopEntry);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battlepay shop entries in %u ms", uint64(_shopEntries.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadProductGroupLocales()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay product group locales ...");

    auto oldMsTime = getMSTime();

    _productGroupLocales.clear();

    auto result = WorldDatabase.PQuery("SELECT GroupID, Locale, Name FROM battlepay_product_group_locales");
    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        auto locale = GetLocaleByName(fields[1].GetString());
        if (locale == LOCALE_none)
            continue;

        auto& data = _productGroupLocales[fields[0].GetUInt32()];
        sObjectMgr->AddLocaleString(fields[2].GetString(), locale, data.Name);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battlepay product group locales strings in %u ms", uint64(_productGroupLocales.size()), GetMSTimeDiffToNow(oldMsTime));
}

void BattlePayDataStoreMgr::LoadDisplayInfoLocales()
{
    TC_LOG_INFO("server.loading", "Loading Battlepay display info locales ...");

    auto oldMsTime = getMSTime();

    _displayInfoLocales.clear();

    auto result = WorldDatabase.PQuery("SELECT Id, Locale, Name1, Name2, Name3, Name4, Name5 FROM battlepay_display_info_locales");
    if (!result)
        return;

    do
    {
        auto fields = result->Fetch();

        auto locale = GetLocaleByName(fields[1].GetString());
        if (locale == LOCALE_none)
            continue;

        auto& data = _displayInfoLocales[fields[0].GetUInt32()];
        sObjectMgr->AddLocaleString(fields[2].GetString(), locale, data.Name1);
        sObjectMgr->AddLocaleString(fields[3].GetString(), locale, data.Name2);
        sObjectMgr->AddLocaleString(fields[4].GetString(), locale, data.Name3);
        sObjectMgr->AddLocaleString(fields[5].GetString(), locale, data.Name4);
        sObjectMgr->AddLocaleString(fields[6].GetString(), locale, data.Name5);
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %lu Battlepay display info locales strings in %u ms", uint64(_displayInfoLocales.size()), GetMSTimeDiffToNow(oldMsTime));
}

std::vector<Battlepay::ProductGroup> const& BattlePayDataStoreMgr::GetProductGroups() const
{
    return _productGroups;
}

std::vector<Battlepay::ShopEntry> const& BattlePayDataStoreMgr::GetShopEntries() const
{
    return _shopEntries;
}

uint32 BattlePayDataStoreMgr::GetProductGroupId(uint32 productId)
{
    for (auto const& shop : _shopEntries)
        if (shop.ProductID == productId)
            return shop.GroupID;
    return 0;
}

std::map<uint32, Battlepay::Product> const& BattlePayDataStoreMgr::GetProducts() const
{
    return _products;
}

bool BattlePayDataStoreMgr::ProductExist(uint32 productID) const
{
    return _products.find(productID) != _products.end();
}

Battlepay::Product const& BattlePayDataStoreMgr::GetProduct(uint32 productID) const
{
    if (ProductExist(productID))
        return _products.at(productID);
    //return {};
    return emptyList;
}

Battlepay::DisplayInfo const* BattlePayDataStoreMgr::GetDisplayInfo(uint32 id) const
{
    if (_displayInfos.find(id) == _displayInfos.end())
        return nullptr;

    return &_displayInfos.at(id);
}

Battlepay::DisplayVisualData const* BattlePayDataStoreMgr::GetDisplayInfoVisuals(uint32 id) const
{
    if (_visualsDatas.find(id) == _visualsDatas.end())
        return nullptr;

    return &_visualsDatas.at(id);
}

Battlepay::ProductGroupLocale const* BattlePayDataStoreMgr::GetProductGroupLocale(uint32 entry) const
{
    return Trinity::Containers::MapGetValuePtr(_productGroupLocales, entry);
}

BattlePayDisplayInfoLocale const* BattlePayDataStoreMgr::GetDisplayInfoLocale(uint32 entry) const
{
    return Trinity::Containers::MapGetValuePtr(_displayInfoLocales, entry);
}
