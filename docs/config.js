var langs = [
  {title: 'English', path: '/home'},
  {title: '中文', path: '/zh/'},
]

self.$config = {
    landing: true,
    repo: 'tboox/tbox',
    twitter: 'waruqi',
    url: 'http://tboox.io',
    'edit-link': 'https://github.com/tboox/tbox/blob/master/docs',
    nav: {
    default: [
      {
        title: 'Home', path: '/home'
      },
      {
        title: 'Manual', path: '/manual'
      },
      {
        title: 'Articles', path: 'http://www.tboox.org/category/#tbox'
      },
      {
        title: 'Feedback', path: 'https://github.com/tboox/tbox/issues'
      },
      {
        title: 'Community', path: 'http://www.tboox.org/forum'
      },
      {
        title: 'English', type: 'dropdown', items: langs, exact: true
      }
    ],
    'zh': [
      {
        title: '首页', path: '/zh/'
      },
      {
        title: '手册', path: '/zh/manual'
      },
      {
        title: '文章', path: 'http://www.tboox.org/cn/category/#tbox'
      },
      {
        title: '反馈', path: 'https://github.com/tboox/tbox/issues'
      },
      {
        title: '社区', path: 'http://www.tboox.org/forum'
      },
      {
        title: '中文', type: 'dropdown', items: langs, exact: true
      }
    ]
  },
  plugins: [
    docsearch({
      apiKey: '',
      indexName: 'tbox',
      tags: ['en', 'zh']
    })
  ]
}
