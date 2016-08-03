#pragma once

template <typename Target, typename Source>
union ForceCast_union {
    Source src;
    Target tgt;
};

template <typename Target, typename Source>
Target force_cast(Source src) {
    ForceCast_union<Target, Source> uni;
    uni.src = src;
    return uni.tgt;
}