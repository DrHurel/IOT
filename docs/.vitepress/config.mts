import { defineConfig } from 'vitepress'

// https://vitepress.dev/reference/site-config
export default defineConfig({
  title: "PlantNanny",
  description: "Smart Plant Care System - Automated watering with environmental monitoring",
  themeConfig: {
    // https://vitepress.dev/reference/default-theme-config
    nav: [
      { text: 'Home', link: '/' },
      { text: 'Guide', link: '/guide/getting-started' },
      { text: 'API Reference', link: '/api/overview' },
      { text: 'Development', link: '/development/architecture' }
    ],

    sidebar: {
      '/guide/': [
        {
          text: 'Introduction',
          items: [
            { text: 'What is PlantNanny?', link: '/guide/what-is-plantnanny' },
            { text: 'Getting Started', link: '/guide/getting-started' },
            { text: 'Quick Start', link: '/guide/quick-start' }
          ]
        },
        {
          text: 'Features',
          items: [
            { text: 'Environmental Monitoring', link: '/guide/features/monitoring' },
            { text: 'Automated Watering', link: '/guide/features/watering' },
            { text: 'Mobile App', link: '/guide/features/mobile-app' },
            { text: 'Plant Configuration', link: '/guide/features/plant-config' }
          ]
        }
      ],
      '/api/': [
        {
          text: 'API Documentation',
          items: [
            { text: 'Overview', link: '/api/overview' },
            { text: 'Authentication', link: '/api/authentication' },
            { text: 'Controller Endpoints', link: '/api/controller' },
            { text: 'Server Endpoints', link: '/api/server' }
          ]
        }
      ],
      '/development/': [
        {
          text: 'Development Guide',
          items: [
            { text: 'Architecture', link: '/development/architecture' },
            { text: 'Project Structure', link: '/development/project-structure' },
            { text: 'Build System', link: '/development/build-system' },
            { text: 'Code Templates', link: '/development/code-templates' }
          ]
        },
        {
          text: 'UI Development',
          items: [
            { text: 'UI Framework', link: '/development/ui/framework' },
            { text: 'Components', link: '/development/ui/components' },
            { text: 'Snapshot Testing', link: '/development/ui/snapshot-testing' },
            { text: 'Quick Reference', link: '/development/ui/snapshot-testing-quick-ref' }
          ]
        },
        {
          text: 'Libraries',
          items: [
            { text: 'Logger System', link: '/development/libraries/logger' },
            { text: 'Service Registry', link: '/development/libraries/service-registry' },
            { text: 'Design Patterns', link: '/development/libraries/patterns' }
          ]
        },
        {
          text: 'Testing',
          items: [
            { text: 'Testing Overview', link: '/development/testing/overview' },
            { text: 'Unit Tests', link: '/development/testing/unit-tests' },
            { text: 'Network Tests', link: '/development/testing/network-tests' }
          ]
        }
      ]
    },

    socialLinks: [
      { icon: 'github', link: 'https://github.com/DrHurel/IOT' }
    ],

    search: {
      provider: 'local'
    },

    footer: {
      message: 'Released under the ISC License.',
      copyright: 'Copyright © 2025 PlantNanny Project'
    }
  }
})
