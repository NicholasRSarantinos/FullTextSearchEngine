# Full text search engine

A variation of the Vector Space Model was implemented from scratch.

Information about the project are available [here](http://www.sarantinos.me/#fulltextsearch)

The Reddit 1.7 billion comment data set is required to test the application. It's available [here](https://www.reddit.com/r/datasets/comments/3bxlg7/i_have_every_publicly_available_reddit_comment/).

The application relies on [RapidJSON](https://github.com/Tencent/rapidjson), [Boost 1.55.0](http://www.boost.org/users/history/version_1_55_0.html), [SIMDComp](https://github.com/lemire/simdcomp), [libtree](https://github.com/fbuihuu/libtree), [libdict](https://github.com/fmela/libdict) and [Intel Thread Building Blocks](https://www.threadingbuildingblocks.org/).

Please modify the file definitions.h to your requirements.

The code we used to benchmark Lucene and compare it's performance with out application is also available. This application depends on [Lucene 7.1](https://lucene.apache.org/core/) and [google gson](https://github.com/google/gson).

## License

This project is licensed under the MIT License.
